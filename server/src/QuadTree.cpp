//
// Created by niels on 07.06.15.
//

#include "QuadTree.h"

QuadTree::QuadTree(const Vector& mPosition, const Vector& mSize, std::function<void(QuadTreeNodePtr, QuadTreeNodePtr)> mCollisionCallback, size_t mMaxAmount, QuadTreePtr mParent)  :
		mParent(mParent), mPosition(mPosition), mSize(mSize), mMaxAmount(mMaxAmount), mCollisionCallback(mCollisionCallback)
{
	printf("Created Region %lf, %lf x %lf, %lf\n", mPosition.x, mPosition.y, mPosition.x+mSize.x, mPosition.y+mSize.y);
	mElements.reserve(mMaxAmount);
}

void QuadTree::doCollisionCheck() const {
	list<QuadTreePtr> neighbours = getNeighbours();
	for(size_t i = 0; i < mElements.size(); i++) {
		QuadTreeNodePtr e1 = mElements[i];
		if(e1->isDeleted()) continue;
		//Start at i + 1 because we already checked these before
		for(size_t j = i + 1; j < mElements.size(); j++) {
			QuadTreeNodePtr e2 = mElements[j];
			if(e2->isDeleted()) continue;
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
		//Pass to neighbours
		for(QuadTreePtr qt : neighbours)
			qt->checkCollision(e1);
	}

	//start checking of children
	if(!mIsLeaf) {
		mChilds[0]->doCollisionCheck();
		mChilds[1]->doCollisionCheck();
		mChilds[2]->doCollisionCheck();
		mChilds[3]->doCollisionCheck();
	}
}

bool QuadTree::add(QuadTreeNodePtr elem) {
	if(isInside(elem->getPosition()) && min(mSize.x, mSize.y) > elem->getSize()) { //Contains point and fits inside
		if(mIsLeaf && mElements.size() < mMaxAmount) { //Still some space left
			mElements.push_back(elem);
			if(!elem->mRegion.expired())
				elem->mRegion.lock()->remove(elem);
			elem->mRegion = shared_from_this();
			printf("Added to Region %lf, %lf x %lf, %lf\n", mPosition.x, mPosition.y, mPosition.x+mSize.x, mPosition.y+mSize.y);
		} else { // No space left
			if(mIsLeaf)
				split();
			//Try to add it to a child
			if(!(mChilds[0]->add(elem) || mChilds[1]->add(elem) || mChilds[2]->add(elem) || mChilds[3]->add(elem))) {
				//Otherwise add it to this node anyway (it is probably to big for the children)
				mElements.push_back(elem);
				if(!elem->mRegion.expired())
					elem->mRegion.lock()->remove(elem);
				elem->mRegion = shared_from_this();
				printf("Added to own Region %lf, %lf x %lf, %lf\n", mPosition.x, mPosition.y, mPosition.x+mSize.x, mPosition.y+mSize.y);
			}
		}
		return true;
	}
	return false;
}

bool QuadTree::remove(QuadTreeNodePtr elem) {
	if(isInside(elem->getPosition())) {
		auto it = mElements.begin();
		while (it != mElements.end()) {
			if (*it == elem)
				break;
			it++;
		}
		if (it != mElements.end()) {
			*it = mElements.back();
			mElements.pop_back();
			return true;
		}
		if (!mIsLeaf) {
			return  mChilds[0]->remove(elem) ||
					mChilds[1]->remove(elem) ||
					mChilds[2]->remove(elem) ||
					mChilds[3]->remove(elem);
		}
	}
	return false;
}

void QuadTree::checkCollision(QuadTreeNodePtr e1) const {
	if(intersects(e1)) { //Only check if the element actually intersects this area
		//Compare with own elements
		for(QuadTreeNodePtr e2 : mElements) {
			if(e2->isDeleted()) continue;
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

void QuadTree::split() {
	if(mIsLeaf) {
		mChilds[NW] = make_shared<QuadTree>(mPosition, mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		mChilds[NE] = make_shared<QuadTree>(mPosition + Vector(mSize.x/2, 0), mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		mChilds[SW] = make_shared<QuadTree>(mPosition + Vector(0, mSize.y/2), mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());
		mChilds[SE] = make_shared<QuadTree>(mPosition + mSize/2, mSize/2, mCollisionCallback, mMaxAmount, shared_from_this());

		vector<QuadTreeNodePtr> oldList = mElements;
		mElements.clear();
		for(QuadTreeNodePtr elem : oldList) {
			if(!(mChilds[0]->add(elem) || mChilds[1]->add(elem) || mChilds[2]->add(elem) || mChilds[3]->add(elem))) {
				mElements.push_back(elem);
			}
		}
		mIsLeaf = false;
	}
}

bool QuadTree::isInside(const Vector& p) const {
	return p.x > mPosition.x && p.x < mPosition.x+mSize.x &&
		   p.y > mPosition.y && p.y < mPosition.y+mSize.y;
}

bool QuadTree::intersects(QuadTreeNodePtr a) const {
	return  a->getPosition().x+a->getSize() > mPosition.x && a->getPosition().x-a->getSize() < mPosition.x+mSize.x &&
			a->getPosition().y+a->getSize() > mPosition.y && a->getPosition().y-a->getSize() < mPosition.y+mSize.y;
}


QuadTreePtr QuadTree::findNorth() const {
	if (mParent) //it is not the head of the tree
	{
		if (shared_from_this() == mParent->mChilds[SE]) return mParent->mChilds[NE];
		if (shared_from_this() == mParent->mChilds[SW]) return mParent->mChilds[NW];
		QuadTreePtr n = mParent->findNorth();
		if(n) {
			if (n->mIsLeaf) return n;
			else if (shared_from_this() == mParent->mChilds[NE]) return n->mChilds[SE];
			else return n->mChilds[SW];
		}
	}
	return QuadTreePtr();
}

QuadTreePtr QuadTree::findSouth() const {
	if (mParent) //it is not the head of the tree
	{
		if (shared_from_this() == mParent->mChilds[NE]) return mParent->mChilds[SE];
		if (shared_from_this() == mParent->mChilds[NW]) return mParent->mChilds[SW];
		QuadTreePtr n = mParent->findSouth();
		if(n) {
			if (n->mIsLeaf) return n;
			else if (shared_from_this() == mParent->mChilds[SE]) return n->mChilds[NE];
			else return n->mChilds[NW];
		}
	}
	return QuadTreePtr();
}

QuadTreePtr QuadTree::findEast() const {
	if (mParent) //it is not the head of the tree
	{
		if (shared_from_this() == mParent->mChilds[NW]) return mParent->mChilds[NE];
		if (shared_from_this() == mParent->mChilds[SW]) return mParent->mChilds[SE];
		QuadTreePtr n = mParent->findEast();
		if(n) {
			if (n->mIsLeaf) return n;
			else if (shared_from_this() == mParent->mChilds[NE]) return n->mChilds[NW];
			else return n->mChilds[SW];
		}
	}
	return QuadTreePtr();
}

QuadTreePtr QuadTree::findWest() const {
	if (mParent) //it is not the head of the tree
	{
		if (shared_from_this() == mParent->mChilds[NE]) return mParent->mChilds[NW];
		if (shared_from_this() == mParent->mChilds[SE]) return mParent->mChilds[SW];
		QuadTreePtr n = mParent->findWest();
		if(n) {
			if (n->mIsLeaf) return n;
			else if (shared_from_this() == mParent->mChilds[NW]) return n->mChilds[NE];
			else return n->mChilds[SE];
		}
	}
	return QuadTreePtr();
}

list<QuadTreePtr> QuadTree::getNeighbours() const {
	if (!mParent) //head as no neigbours
		return list<QuadTreePtr>();

	QuadTreePtr northeast;
	QuadTreePtr northwest;
	QuadTreePtr southeast;
	QuadTreePtr southwest;
	QuadTreePtr north = findNorth();
	QuadTreePtr west = findWest();
	QuadTreePtr south = findSouth();
	QuadTreePtr east = findEast();

	list<QuadTreePtr> res;

	if(west)
		res.push_back(west);
	if(east)
		res.push_back(east);
	if(north)
		res.push_back(north);
	if(south)
		res.push_back(south);

	if(north) {
		northeast = north->findEast();
		if(northeast)
			res.push_back(northeast);
		northwest = north->findWest();
		if(northwest)
			res.push_back(northwest);
	}

	if(south) {
		southeast = south->findEast();
		if(southeast)
			res.push_back(southeast);
		southwest = south->findWest();
		if(southwest)
			res.push_back(southwest);
	}

	return res;
}

void QuadTreeNode::updateRegion() {
	//if(mRegion.expired()) return;
	if(!mRegion.lock()->isInside(mPosition)) {
		list<QuadTreePtr> regions = mRegion.lock()->getNeighbours();
		for (QuadTreePtr region : regions) {
			if (region->add(shared_from_this()))
				return;
		}
		//Should never appear
		fprintf(stderr, "Can not find Region for position %.0lf, %.0lf\n", mPosition.x, mPosition.y);
	}
}
