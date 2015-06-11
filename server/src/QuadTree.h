//
// Created by niels on 07.06.15.
//

#ifndef SERVER_QUADTREE_H
#define SERVER_QUADTREE_H

#include "GlobalDefs.h"
#include "Vector.h"

class QuadTreeNode : public std::enable_shared_from_this<QuadTreeNode> {
friend class QuadTree;
protected:
	Vector mPosition;
	double mSize = 0;

private:
	QuadTree* mRegion = NULL;
	volatile bool mDeleted = false;

public:
	QuadTreeNode() {}
	QuadTreeNode(const Vector& mPosition, double mSize) : mPosition(mPosition), mSize(mSize) { }

	virtual ~QuadTreeNode() {}

	const Vector& getPosition() const { return mPosition; }
	double getSize() const { return mSize; }
	QuadTreePtr getRegion() const { return mRegion; }
	void setRegion(QuadTreePtr r)  { mRegion = r; }

	inline bool intersect(QuadTreeNodePtr other) {
		double dist = getPosition().distanceSquared(other->getPosition());
		return dist <= getSize() * getSize() || dist <= other->getSize() * other->getSize();
	}
	void updateRegion();

	bool isDeleted() const { return mDeleted; }
	void markDeleted() { mDeleted = true; }
};


class QuadTree : public std::enable_shared_from_this<QuadTree> {
friend class QuadTreeNode;
private:
	enum {
		NW,
		NE,
		SW,
		SE
	};
private:
	QuadTreePtr mParent = NULL;
	Vector mPosition;
	Vector mSize;
	size_t mMaxAmount;

	std::function<void (QuadTreeNodePtr, QuadTreeNodePtr)> mCollisionCallback;
	vector<QuadTreeNodePtr> mElements;
	QuadTreePtr mChilds[4];
	volatile bool mIsLeaf = true;

	mutex mMutex;

public:
	QuadTree(const Vector& mPosition, const Vector& mSize, std::function<void (QuadTreeNodePtr, QuadTreeNodePtr)> mCollisionCallback, size_t mMaxAmount = 5, QuadTreePtr mParent = NULL);

	void doCollisionCheck();
	bool add(QuadTreeNodePtr elem);
	bool remove(QuadTreeNodePtr elem);

	size_t getElementCount() const;
	size_t getChildCount() const;
private:

	QuadTreePtr getHead() { return mParent ? mParent->getHead() : this; }

	void checkCollision(QuadTreeNodePtr elem);
	void split();
	void combine();

	bool isInside(QuadTreeNodePtr elem) const;
	bool intersects(QuadTreeNodePtr elem) const;

	QuadTreePtr findNorth() const;
	QuadTreePtr findSouth() const;
	QuadTreePtr findEast() const;
	QuadTreePtr findWest() const;
	list<QuadTreePtr> getNeighbours() const;

};


#endif //SERVER_QUADTREE_H
