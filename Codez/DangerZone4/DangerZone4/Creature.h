#ifndef CREATURE_H
#define CREATURE_H

#include <vector>
#include <iostream>

#include "Vector2.h"
using namespace std;

class Creature {
public:

	const float robot_speed = 0.5f;

	Vector2 location;
	float direction;
	vector<Vector2> baseVertices;

	// BRAINZZ
	float k11, k12, k21, k22;


	Creature(Vector2 location, float k11, float k12, float k21, float k22);

	vector<Vector2> getVertices();

	// Takes in the values at both of the sensors and determines where
	// the robot should stand, and what direction it should face
	void processSensors(float left, float right);
	vector<Vector2> getSensors();
};

#endif