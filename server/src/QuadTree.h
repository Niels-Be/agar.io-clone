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
	double mSize;

private:
	std::weak_ptr<QuadTree> mRegion;
	bool mDeleted = false;

public:
	QuadTreeNode() : mSize(0) {}
	QuadTreeNode(const Vector& mPosition, double mSize) : mPosition(mPosition), mSize(mSize) { }

	virtual ~QuadTreeNode() {}

	const Vector& getPosition() const { return mPosition; }
	double getSize() const { return mSize; }

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
	QuadTreePtr mParent;
	Vector mPosition;
	Vector mSize;
	size_t mMaxAmount;

	std::function<void (QuadTreeNodePtr, QuadTreeNodePtr)> mCollisionCallback;
	vector<QuadTreeNodePtr> mElements;
	QuadTreePtr mChilds[4];
	bool mIsLeaf = true;

public:
	QuadTree(const Vector& mPosition, const Vector& mSize, std::function<void (QuadTreeNodePtr, QuadTreeNodePtr)> mCollisionCallback, size_t mMaxAmount = 5, QuadTreePtr mParent = QuadTreePtr());

	void doCollisionCheck() const;
	bool add(QuadTreeNodePtr elem);
	bool remove(QuadTreeNodePtr elem);

private:

	void checkCollision(QuadTreeNodePtr elem) const;
	void split();

	bool isInside(const Vector& point) const;
	bool intersects(QuadTreeNodePtr) const;

	QuadTreePtr findNorth() const;
	QuadTreePtr findSouth() const;
	QuadTreePtr findEast() const;
	QuadTreePtr findWest() const;
	list<QuadTreePtr> getNeighbours() const;

};


#endif //SERVER_QUADTREE_H
