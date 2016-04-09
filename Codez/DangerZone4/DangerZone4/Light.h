#ifndef LIGHT_H
#define LIGHT_H

#include "Vector2.h"
#include <vector>

class Light {
public:
	Vector2 location;
	float red, green, blue;

	Light(Vector2 location, float red, float green, float blue);
};

#endif