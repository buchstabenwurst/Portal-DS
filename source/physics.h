#ifndef PHYSICS_H
#define PHYSICS_H
#include "main.h"

#define EPSILON 0.00390625
//friction value 0.1 to 1 is least to most
#define FRICTION 0.85
#define AIR_FRICTION 0.996
//gravity in coordinate-units/frame²
#define GRAVITY 0.04

float dot(Vector3 a, Vector3 b);
Vector3 Normalize(Vector3 vector);
float getDistance(Vector3 pos1, Vector3 pos2);
void addHitbox(Vector3 size, Vector3* position, Vector3* rotation, bool isDynamic);
void shootPortal(bool Portal);
void doPhysics(void);

#endif