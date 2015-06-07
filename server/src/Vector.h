//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_VECTOR_H
#define AGARIO_VECTOR_H

#include <math.h>

class Vector {
public:
	double x;
	double y;

	static const Vector ZERO;
	static const Vector DIRECTION_X;
	static const Vector DIRECTION_Y;
	static const Vector DIRECTION_NEG_X;
	static const Vector DIRECTION_NEG_Y;

	Vector() : x(0), y(0) { }

	Vector(double x, double y) : x(x), y(y) { }

	static Vector FromAngle(double angle) {
		return Vector(
				cos(angle),
				sin(angle)
		);
	}

	Vector operator +(const Vector& other) const {
		return Vector(x + other.x, y + other.y);
	}

	Vector operator +(double v) const {
		return Vector(x + v, y + v);
	}

	Vector operator -(const Vector& other) const {
		return Vector(x - other.x, y - other.y);
	}

	Vector operator -(double v) const {
		return Vector(x - v, y - v);
	}

	Vector operator *(const Vector& other) const {
		return Vector(x * other.x, y * other.y);
	}

	Vector operator *(double v) const {
		return Vector(x * v, y * v);
	}

	Vector operator /(const Vector& other) const {
		return Vector(x / other.x, y / other.y);
	}

	Vector operator /(double v) const {
		return Vector(x / v, y / v);
	}

	Vector& operator +=(const Vector& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector& operator +=(double v) {
		x += v;
		y += v;
		return *this;
	}

	Vector& operator -=(const Vector& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector& operator -=(double v) {
		x -= v;
		y -= v;
		return *this;
	}

	Vector& operator *=(const Vector& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vector& operator *=(double v) {
		x *= v;
		y *= v;
		return *this;
	}

	Vector& operator /=(const Vector& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	Vector& operator /=(double v) {
		x /= v;
		y /= v;
		return *this;
	}

	Vector& normalise() {
		double len = length();
		x /= len;
		y /= len;
		return *this;
	}

	Vector normaliseCopy() const {
		Vector v(x, y);
		v.normalise();
		return v;
	}

	inline double length() const {
		return sqrt(x * x + y * y);
	}

	inline double lengthSquared() const {
		return x * x + y * y;
	}

	inline double distance(const Vector& other) const {
		return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
	}

	inline double distanceSquared(const Vector& other) const {
		return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
	}

	double angle() const {
		return atan2(y, x);
	}


};


#endif //AGARIO_VECTOR_H
