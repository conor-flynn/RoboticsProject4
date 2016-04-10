#include "Creature.h"



Creature::Creature(Vector2 location, float direction, float k11, float k12, float k21, float k22) {
	this->location = location;
	this->direction = direction;
	this->k11 = k11;
	this->k12 = k12;
	this->k21 = k21;
	this->k22 = k22;

	baseVertices.push_back(Vector2(-5, 0));
	baseVertices.push_back(Vector2(0, 5));
	baseVertices.push_back(Vector2(5, 0));


	// Super artistic design for the robot.
	/*baseVertices.push_back(Vector2(15, 5));
	baseVertices.push_back(Vector2(18, 20));
	baseVertices.push_back(Vector2(20, 20));
	baseVertices.push_back(Vector2(23, 5));
	baseVertices.push_back(Vector2(40, 5));
	baseVertices.push_back(Vector2(40, 10));
	baseVertices.push_back(Vector2(50, 10));
	baseVertices.push_back(Vector2(50, -10));
	baseVertices.push_back(Vector2(40, -10));
	baseVertices.push_back(Vector2(40, -5));
	baseVertices.push_back(Vector2(-40, -5));
	baseVertices.push_back(Vector2(-40, -10));
	baseVertices.push_back(Vector2(-50, -10));
	baseVertices.push_back(Vector2(-50, 10));
	baseVertices.push_back(Vector2(-40, 10));
	baseVertices.push_back(Vector2(-40, 5));
	baseVertices.push_back(Vector2(-23, 5));
	baseVertices.push_back(Vector2(-20, 20));
	baseVertices.push_back(Vector2(-18, 20));
	baseVertices.push_back(Vector2(-15, 5));*/
}

vector<Vector2> Creature::getVertices() {
	vector<Vector2> result;
	for (Vector2 v : baseVertices) {
		Vector2 res = v.rotate(direction);
		Vector2 res2 = res.add(location);
		//result.push_back(v.rotate(direction).add(location));
		result.push_back(res2);
	}
	return result;
}

void Creature::processSensors(float left, float right) {
	

	float left_wheel = (k11*left) + (k12*right);
	float right_wheel = (k21*left) + (k22*right);

	direction += (right_wheel - left_wheel)*10;
	float speed = right_wheel + left_wheel;

	// '3' is hardcoded. Just controls the value of the total speed of the robots
	// The value of 'speed' 
	Vector2 move = Vector2(0.0f, robot_speed*speed);

	move = move.rotate(direction);
	location = Vector2::add(location, move);

}

vector<Vector2> Creature::getSensors() {
	// Get the two Vector2's of the sensors

	// These hardcoded values are dependent on the design of the robot.
	// Technically they can be set to anything, but they should match the robot a little
	float xDistance = 5;
	float yDistance = 10;
	Vector2 left  = Vector2( xDistance, yDistance).rotate(direction).add(location);
	Vector2 right = Vector2(-xDistance, yDistance).rotate(direction).add(location);

	vector<Vector2> result;
	result.push_back(left);
	result.push_back(right);
	return result;
}