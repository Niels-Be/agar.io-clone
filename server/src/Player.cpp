//
// Created by niels on 02.06.15.
//

#include "Player.h"
#include "Ball.h"
#include "Gamefield.h"
#include "Network/Client.h"
#include "Network/AgarPackets.h"

using std::placeholders::_1;
using std::placeholders::_2;

Player::Player(GamefieldPtr mGamefield, ClientPtr mClient, const String& mColor, const String& mName) :
		mClient(mClient), mGamefield(mGamefield), mColor(mColor), mName(mName)
{
	//Set Callbacks
	mClient->on(PID_UpdateTarget, std::bind(&Player::onUpdateTarget, this, _1, _2));
	mClient->on(PID_SplitUp, std::bind(&Player::onSplitUp, this, _1, _2));
	mClient->on(PID_Shoot, std::bind(&Player::onShoot, this, _1, _2));
}

void Player::setTarget(const Vector& target) {
	for (BallPtr ball : mBalls) {
		Vector t = target - (ball->getPosition() - mPosition);
		if (t.lengthSquared() < ball->getSize() * ball->getSize() / 4)
			ball->setDirection(Vector::ZERO, false); //Stop moving
		else
			ball->setDirection(Vector::FromAngle(t.angle()));
	}
}

void Player::splitUp(const Vector& target) {
	list<BallPtr> balls = mBalls; //Store list here because new balls will be added
	for (BallPtr ball : balls) {
		if (ball->getMass() > mGamefield->getOptions().player.minSplitMass) {
			Vector t = target - (ball->getPosition() - mPosition);
			ball->splitUp(Vector::FromAngle(t.angle()));
		}
	}
	updateClient();
}

void Player::shoot(const Vector& target) {
	for (BallPtr ball : mBalls) {
		if (ball->getMass() > mGamefield->getOptions().player.minSplitMass) {
			Vector t = target - (ball->getPosition() - mPosition);
			ball->shoot(Vector::FromAngle(t.angle()));
		}
	}
	updateClient();
}

void Player::addBall(BallPtr ball) {
	mBalls.push_back(ball);
}

void Player::removeBall(uint32_t ball) {
	for (auto it = mBalls.begin(); it != mBalls.end(); it++) {
		if ((*it)->getId() == ball) {
			mBalls.erase(it);
			break;
		}
	}
	if (mBalls.empty()) {
		mClient->emit(std::make_shared<EmptyPacket<PID_RIP> >());
	}
}

void Player::update(double /*timediff*/) {
	mPosition = Vector::ZERO;
	double size = 0;
	//Center of Player in the middle of its balls weighted by size
	for (BallPtr ball : mBalls) {
		mPosition += ball->getPosition() * ball->getSize();
		size += ball->getSize();
	}
	mPosition /= size;
	setTarget(mTarget);
}

void Player::onSplitUp(ClientPtr client, PacketPtr packet) {
	splitUp(mTarget);
}

void Player::onShoot(ClientPtr client, PacketPtr packet) {
	shoot(mTarget);
}

void Player::onUpdateTarget(ClientPtr client, PacketPtr packet) {
	auto p = std::dynamic_pointer_cast<StructPacket<PID_UpdateTarget, TargetPacket> >(packet);
	mTarget = Vector((*p)->x, (*p)->y);
	setTarget(mTarget);
}

void Player::updateClient() {
	mClient->emit(std::make_shared<PlayerUpdatePacket>(shared_from_this()));
}
