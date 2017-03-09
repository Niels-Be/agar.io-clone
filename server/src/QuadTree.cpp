//************************************************************************************************//
// Created by niels on 07.06.15.                                                                  //
//************************************************************************************************//
// Edits Performed by Mostafa Okasha on March 07 and March 08 - 2017 to improve the 
// documentation of this file
//************************************************************************************************//
// The comments were created to the best of my ability to understand the code as there was very
// little code documentation provided by the creater of the original document.
//************************************************************************************************//

#include "QuadTree.h" //  Includes the C++ function declarations and macro definitions to be shared

//***************************************************************************************************************************************//
// Note: If you do not know what a QuadTree is it is a tree like data structure where by each node has 4 children.
// This expands from the smallest data size up to the full scale of the data.
//***************************************************************************************************************************************//

/* This is the default class constructor of the QuadTree class:
Class QuadTree was instantiated in QuadTree.h. We are setting the resources of the class to construct the class with the given parameter
types. This calls the main function QuadTree and feeds it the required parameters for the data structure to be formed. It subsequintially
calls each function and returns the values as the parameters.
*/
QuadTree::QuadTree(const Vector& mPosition, const Vector& mSize, std::function<void(QuadTreeNodePtr, QuadTreeNodePtr)> mCollisionCallback, size_t mMaxAmount, QuadTreePtr mParent)  :
		mParent(mParent), mPosition(mPosition), mSize(mSize), mMaxAmount(mMaxAmount), mCollisionCallback(mCollisionCallback)
{
	//printf("Created Region %lf, %lf x %lf, %lf\n", mPosition.x, mPosition.y, mPosition.x+mSize.x, mPosition.y+mSize.y);
	
	// Requests that mElements vector capacity is at least mMaxAmount. If not then it increases storage capacity for that vector
	mElements.reserve(mMaxAmount);     
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
 Function checks to see if the current object has touched another object of the same class i.e: to see if they have collided
*/
void QuadTree::doCollisionCheck() {
	list<QuadTreePtr> neighbours = getNeighbours();   // We need all the surrounding neighbours to check for collisions

	//start checking of children
	if(!mIsLeaf) {   // mIsLeaf checks to see if it's the last node of the tree (a leaf). Returns true if so.
		mChilds[0]->doCollisionCheck();             // If not then checks if all 4 children have collided with any object i.e calls itself back
		mChilds[1]->doCollisionCheck();
		mChilds[2]->doCollisionCheck();
		mChilds[3]->doCollisionCheck();
	}
	
	vector<QuadTreeNodePtr> oldList;
	{
		lock_guard<mutex> _lock(mMutex);      // Safety lock for multithreading reading
		oldList = mElements;                  // We set our current elemnts to the old elements before creating a node shift
	}

	for(size_t i = 0; i < oldList.size(); i++) {      // Keep repeating i times where i is the size of the mElements
		QuadTreeNodePtr& e1 = oldList[i];                 // map element1 to the oldList
		if(e1->isDeleted()) continue;                    // Keep going if first element is non-existant
		//Start at i + 1 because we already checked these before
		for(size_t j = i + 1; j < oldList.size(); j++) {        // same recursion as above.  
			QuadTreeNodePtr& e2 = oldList[j];                   // Map element2 to the oldList
			assert(e1 != e2);                                   
			if(e1->isDeleted() || e2->isDeleted()) continue;        // only continue when both elements have been deleted
			if(e1->intersect(e2)) {                                 // if so then we check for a collision between e1 and e2
				mCollisionCallback(e1, e2);
			}
		}
		//Pass to children
		if(!mIsLeaf) {                          // mIsLeaf checks to see if it's the last node of the tree (a leaf). Returns true if so. 
			mChilds[0]->checkCollision(e1);   // for each node checks to see if any elements intersect with the area e1
			mChilds[1]->checkCollision(e1);
			mChilds[2]->checkCollision(e1);
			mChilds[3]->checkCollision(e1);
		}
		//Pass to neighbours
		for(QuadTreePtr& qt : neighbours)  //map out elements to the neighbouring objects so they can check for collisions with the area
			qt->checkCollision(e1);
	}
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
 The add cuntion adds an element to the children of the parent is capable of supplying space to the element
*/
bool QuadTree::add(QuadTreeNodePtr elem) { // will always return true if there is space to add elements otherwise it returns false
	if(isInside(elem)) { //Contains point and fits inside
// mIsLeaf checks to see if it's the last node of the tree (a leaf). Returns true if so.
		if(mIsLeaf && mElements.size() < mMaxAmount) { // if there is still space left to add elements then do the following
			{
				lock_guard<mutex> _lock(mMutex);         // lock it up for safety
				/*for(QuadTreeNodePtr& e : mElements)
					if(e == elem)
						printf("Double Element in TreeNode!!! %p\n", e);*/

				mElements.push_back(elem);                 // shift the elements of the node back 
			}
			if(elem->mRegion)             //this checks to see if elem exists in the region if so it will delete it
				elem->mRegion->remove(elem);
			elem->mRegion = this;
			//printf("Added to Region %lf, %lf x %lf, %lf\n", mPosition.x, mPosition.y, mPosition.x+mSize.x, mPosition.y+mSize.y);
		} else { // No space left
	// mIsLeaf checks to see if it's the last node of the tree (a leaf). Returns true if so.
			if(mIsLeaf)      //check to see if node is last node in the tree
				split();
			//Try to add it to a child
			if(!(mChilds[0]->add(elem) || mChilds[1]->add(elem) || mChilds[2]->add(elem) || mChilds[3]->add(elem))) {
				//Otherwise add it to this node anyway (it is probably to big for the children)
				{
					lock_guard<mutex> _lock(mMutex); //same as above
					/*for(QuadTreeNodePtr& e : mElements)
						if(e == elem)
							printf("Double2 Element in TreeNode!!! %p\n", e);*/

					mElements.push_back(elem);
				}
				if(elem->mRegion)             //same as above
					elem->mRegion->remove(elem);
				elem->mRegion = this;
				//printf("Added to own Region %lf, %lf x %lf, %lf\n", mPosition.x, mPosition.y, mPosition.x+mSize.x, mPosition.y+mSize.y);
			}
		}
		return true;
	}
	return false;
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
   This function simply removes the element in the node and when it does it will continue doing so to all children of that node
*/
bool QuadTree::remove(QuadTreeNodePtr elem) {      // attempt and remove elem
	//if(isInside(elem)) {
		bool found = false;
		{
			lock_guard<mutex> _lock(mMutex);    // safety first

			auto it = mElements.begin();       // checks for repetition of the element
			while (it != mElements.end()) {
				if (*it == elem)
					break;
				it++;
			}
			if (it != mElements.end()) {       // if the required element exists then it is removed
				*it = mElements.back();
				mElements.pop_back();
				found = true;            // sets found to true upon successful popping
			}
		}
		if(found) {                          //after removal checks to see if the node is empty.
			if(mElements.empty()) {
				if(mIsLeaf && mParent)
					mParent->combine();
				else
					combine();                   // if no children exist then the parent has no children 
			}
			return true;
		} else
		if (!mIsLeaf) {                                 // constantly remove all elements of all children
			return  mChilds[0]->remove(elem) ||
					mChilds[1]->remove(elem) ||
					mChilds[2]->remove(elem) ||
					mChilds[3]->remove(elem);
		}
	//}
	return false;
}

//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
     returns the number of elements under each child 
*/
size_t QuadTree::getElementCount() const {
	if(mIsLeaf)                       // if the elemeent is the last node then the total size is it's size
		return mElements.size();
	return mElements.size() +                       //sums all the number of elements and returns that value
		   mChilds[0]->getElementCount() +
		   mChilds[1]->getElementCount() +
		   mChilds[2]->getElementCount() +
		   mChilds[3]->getElementCount();
}

//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
   returns the total number of existing children under that node
*/
size_t QuadTree::getChildCount() const {
	return mIsLeaf ? 1 : 1 +                                //returns 1 if the node is the last node otherwise gets the total number of children
						 mChilds[0]->getChildCount() +
						 mChilds[1]->getChildCount() +
						 mChilds[2]->getChildCount() +
						 mChilds[3]->getChildCount();
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
   This function is used under the doCollisionCheck and only checks for intersections
*/
void QuadTree::checkCollision(QuadTreeNodePtr e1) {
	if(intersects(e1)) { //Only check if the element actually intersects this area
		vector<QuadTreeNodePtr> oldList;
		{
			lock_guard<mutex> _lock(mMutex);
			oldList = mElements;
		}
		//Compare with own elements
		for(QuadTreeNodePtr e2 : oldList) {
			assert(e1 != e2);
			if(e1->isDeleted() || e2->isDeleted()) continue;
			if(e1->intersect(e2)) {
				mCollisionCallback(e1, e2);
			}
		}
		//Pass to children
		if(!mIsLeaf) {
			mChilds[0]->checkCollision(e1);
			mChilds[1]->checkCollision(e1);
			mChilds[2]->checkCollision(e1);
			mChilds[3]->checkCollision(e1);
		}
	}
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
  This function creates a new set of children in all 4 opposing vectors that will cause the mesh of the vectors to look exactly the same
*/
void QuadTree::split() {
	if(mIsLeaf) {
		//mChilds[NW] = make_shared<QuadTree>(mPosition, mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		//mChilds[NE] = make_shared<QuadTree>(mPosition + Vector(mSize.x/2, 0), mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		//mChilds[SW] = make_shared<QuadTree>(mPosition + Vector(0, mSize.y/2), mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		//mChilds[SE] = make_shared<QuadTree>(mPosition + mSize/2, mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		mChilds[NW] = new QuadTree(mPosition, mSize/2, mCollisionCallback, mMaxAmount, this);
		mChilds[NE] = new QuadTree(mPosition + Vector(mSize.x/2, 0), mSize/2, mCollisionCallback, mMaxAmount, this);
		mChilds[SW] = new QuadTree(mPosition + Vector(0, mSize.y/2), mSize/2, mCollisionCallback, mMaxAmount, this);
		mChilds[SE] = new QuadTree(mPosition + mSize/2, mSize/2, mCollisionCallback, mMaxAmount, this);
        // creates 4 children in 4 different directions (hence the different addition)
		vector<QuadTreeNodePtr> oldList;  //creates a vector that will map these values
		{
			lock_guard<mutex> _lock(mMutex);
			oldList = std::move(mElements);
			mElements.clear();
		}
		for(QuadTreeNodePtr& elem : oldList) {
			if(!(mChilds[0]->add(elem) || mChilds[1]->add(elem) || mChilds[2]->add(elem) || mChilds[3]->add(elem))) {
				lock_guard<mutex> _lock(mMutex);
				mElements.push_back(elem);
			}
		}
		mIsLeaf = false;
	}
}

//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
   combines two nodes if the node will have the capacity to do so
*/
void QuadTree::combine() {
	if(!mIsLeaf) {
		if(getElementCount() < mMaxAmount / 2) {
		//this is very important to note. since it's combining two elements, the total element size 
		// cannot surpass half of the size of that element so that both elements can successfully fit inside
			printf("Combining Nodes %ld Elems: %ld\n", getChildCount(), getElementCount());
			for(int i = 0; i < 4; i++) //combines all 4 children and all subsequent children of each child. That's why the function calls itself
										// recursively
				mChilds[i]->combine();

			lock_guard<mutex> _lock(mMutex); // lock and load
			for(int i = 0; i < 4; i++)
			{
				lock_guard<mutex> _lock(mChilds[i]->mMutex);
				// Inserts the elements into each child as it combines everything
				mElements.insert(mElements.end(), mChilds[i]->mElements.begin(), mChilds[i]->mElements.end());
				delete mChilds[i]; //delete all the elements after adding them together
				mChilds[i] = NULL;  // set the elements to being empty
			}
			for(QuadTreeNodePtr& e : mElements) //checks to see if combination was successful
				e->mRegion = this;
			mIsLeaf = true;
			printf("Done Combining %ld Elems: %ld\n", getChildCount(), getElementCount());
		}
	}
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
 Checks to see if the element exists inside the node and if so then it returns the position of that element in each direction
*/
bool QuadTree::isInside(QuadTreeNodePtr a) const {
	return a->getPosition().x >= mPosition.x && a->getPosition().x <= mPosition.x+mSize.x &&
		   a->getPosition().y >= mPosition.y && a->getPosition().y <= mPosition.y+mSize.y &&
		   min(mSize.x, mSize.y) >= a->getSize();
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
  Creturns the value of intersection level between two elements
*/
bool QuadTree::intersects(QuadTreeNodePtr a) const {
	return  a->getPosition().x+a->getSize() >= mPosition.x && a->getPosition().x-a->getSize() <= mPosition.x+mSize.x &&
			a->getPosition().y+a->getSize() >= mPosition.y && a->getPosition().y-a->getSize() <= mPosition.y+mSize.y;
}

//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
 Looks to see where the top direction is and returns that value
*/
QuadTreePtr QuadTree::findNorth() const {
	if (mParent) //it is not the head of the tree - 
	{            // we initially compare the directions and return the values that would give an adjustment to that
		if (this == mParent->mChilds[SE]) return mParent->mChilds[NE];
		if (this == mParent->mChilds[SW]) return mParent->mChilds[NW];
		QuadTreePtr n = mParent->findNorth();     //constantly do this until we find north
		if(n) {
			if (n->mIsLeaf) return n;
			else if (this == mParent->mChilds[NE]) return n->mChilds[SE];
			else return n->mChilds[SW];
		}
	}
	return QuadTreePtr();
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
Looks to see where the bottom direction is and returns that value
*/
QuadTreePtr QuadTree::findSouth() const {
	if (mParent) //it is not the head of the tree
	{      // we initially compare the directions and return the values that would give an adjustment to that
		if (this == mParent->mChilds[NE]) return mParent->mChilds[SE];
		if (this == mParent->mChilds[NW]) return mParent->mChilds[SW];
		QuadTreePtr n = mParent->findSouth();     //constantly do this until we find south
		if(n) {
			if (n->mIsLeaf) return n;
			else if (this == mParent->mChilds[SE]) return n->mChilds[NE];
			else return n->mChilds[NW];
		}
	}
	return QuadTreePtr();
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
 Looks to see where the right direction is and returns that value
*/
QuadTreePtr QuadTree::findEast() const {
	if (mParent) //it is not the head of the tree
	{   // we initially compare the directions and return the values that would give an adjustment to that
		if (this == mParent->mChilds[NW]) return mParent->mChilds[NE];
		if (this == mParent->mChilds[SW]) return mParent->mChilds[SE];
		QuadTreePtr n = mParent->findEast();       //constantly do this until we find East
		if(n) {
			if (n->mIsLeaf) return n;
			else if (this == mParent->mChilds[NE]) return n->mChilds[NW];
			else return n->mChilds[SW];
		}
	}
	return QuadTreePtr();
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
  Looks to see where the left direction is and returns that value
*/
QuadTreePtr QuadTree::findWest() const {
	if (mParent) //it is not the head of the tree
	{    // we initially compare the directions and return the values that would give an adjustment to that
		if (this == mParent->mChilds[NE]) return mParent->mChilds[NW];
		if (this == mParent->mChilds[SE]) return mParent->mChilds[SW];
		QuadTreePtr n = mParent->findWest();    //constantly do this until we find West
		if(n) {
			if (n->mIsLeaf) return n;
			else if (this == mParent->mChilds[NW]) return n->mChilds[NE];
			else return n->mChilds[SE];
		}
	}
	return QuadTreePtr();
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
    This function checks if there are any existing neighbours and returns them one by one 
*/
list<QuadTreePtr> QuadTree::getNeighbours() const {
	if (!mParent) // The top of the tree will have no neighbours 
		return list<QuadTreePtr>();

	QuadTreePtr northeast;      // creates an object of all the direction types and finds all 4 main directions for that ptr
	QuadTreePtr northwest;
	QuadTreePtr southeast;
	QuadTreePtr southwest;
	QuadTreePtr north = findNorth();
	QuadTreePtr west = findWest();
	QuadTreePtr south = findSouth();
	QuadTreePtr east = findEast();

	list<QuadTreePtr> res;

	if(west)                     //push all the main direction values
		res.push_back(west);
	if(east)
		res.push_back(east);
	if(north)
		res.push_back(north);
	if(south)
		res.push_back(south);

	if(north) {                           //looks for interacting neighbours by comparing the direction of the vectors
		northeast = north->findEast();
		if(northeast)
			res.push_back(northeast);
		northwest = north->findWest();
		if(northwest)
			res.push_back(northwest);          // if so it pushes that value
	}

	if(south) {                               //looks for interacting neighbours by comparing the direction of the vectors
		southeast = south->findEast();
		if(southeast)
			res.push_back(southeast);
		southwest = south->findWest();
		if(southwest)
			res.push_back(southwest);               // if so it pushes that valu
	}

	return res;
}
//***************************************************************************************************************************************//
//***************************************************************************************************************************************//
/* Function Summary:
  Checks to see if the element exists in any regions, if it is not in a region it will inform us
*/   // This code does not work well for now
void QuadTreeNode::updateRegion() {
	if(!mRegion) {
		printf("Element is not in a Region\n");
		return;
	}
	if(!mRegion->isInside(this)) {
		/* //Does not work because the element may skip regions in a lag
		list<QuadTreePtr> regions = mRegion->getNeighbours();
		for (QuadTreePtr region : regions) {
			if (region->add(this))
				return;
		}*/
		if(!mRegion->getHead()->add(this)) {
			//Should never appear
			fprintf(stderr, "Can not find Region for position %.0lf, %.0lf\n", mPosition.x, mPosition.y);
			assert(false);
		}
	}
}
