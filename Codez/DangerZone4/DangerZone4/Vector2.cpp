#include "Vector2.h"

Vector2::Vector2() {
	x = 0;
	y = 0;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2::Vector2(const Vector2 & vector) {
	this->x = vector.getX();
	this->y = vector.getY();
}

float Vector2::getX() const { return this->x; }
float Vector2::getY() const { return this->y; }
void Vector2::setX(float x) {
	this->x = x;
}
void Vector2::addX(float x) {
	this->x += x;
}
void Vector2::setY(float y) {
	this->y = y;
}
void Vector2::addY(float y) {
	this->y += y;
}







Vector2 Vector2::subtract(const Vector2 &b, const Vector2 &a) {
	return Vector2(b.getX() - a.getX(), b.getY() - a.getY());
}
Vector2 Vector2::add(const Vector2 &a, const Vector2 &b) {
	return Vector2(a.getX() + b.getX(), a.getY() + b.getY());
}
float Vector2::dot(const Vector2 &a, const Vector2 &b) {
	return (a.getX() * b.getX()) + (a.getY() * b.getY());
}

Vector2 Vector2::scale(float value) const {
	return Vector2(this->getX() * value, this->getY() * value);
}

Vector2 Vector2::rotate(float value) const {
	float rad = (value / 180.0f) * 3.1415f;;
	float newx = (getX() * cos(rad)) - (getY() * sin(rad));
	float newy = (getX() * sin(rad)) + (getY() * cos(rad));
	return Vector2(newx, newy);
}

Vector2 Vector2::add(const Vector2 & a) const {
	return add(*this, a);
}
