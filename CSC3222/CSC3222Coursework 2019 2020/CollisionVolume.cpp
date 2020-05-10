#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

CollisionVolume::CollisionVolume()
{
}

CollisionVolume::CollisionVolume(RigidBody* rb, int shape, bool dyna, Vector2* position)
{
	this->rb = rb;
	this->shape = shape;
	dynamic = dyna;
	pos = position;
	counter = 0;
}

CollisionVolume::CollisionVolume(RigidBody* rb, float w, float h, int shape, bool dyna, Vector2* position)
{
	this->rb = rb;
	width = w;
	height = h;
	this->shape = shape;
	dynamic = dyna;
	pos = position;
	counter = 0;
}


CollisionVolume::~CollisionVolume()
{
}

float CollisionVolume::getMax() {
	return pos->x + width / 2;
}

float CollisionVolume::getMin() {
	return pos->x - width / 2;
}

bool CollisionVolume::SameShape(CollisionVolume& rhs) {

	if (this->shape == 0) { //if Sphere
		float distance = (*pos - *(rhs.pos)).Length(); 
		//Calculate the distance
		if (distance <= (width / 2) + (rhs.width / 2)) { 
			//Compare this distance with the sum of their radius
			return true;
		}
	}
	else { //if Square or box
		float distanceX = abs(pos->x - rhs.pos->x);
		float distanceY = abs(pos->y - rhs.pos->y);
		if (distanceX <= (width / 2 + rhs.width / 2)) {
			if (distanceY <= (height / 2) + (rhs.height / 2)) {
				return true;
			}
		}
	}
	return false;
}

bool CollisionVolume::SameShape(CollisionVolume& rhs, CollisionPair* cp) {
	if (!this->collider || !rhs.collider)
		return false;

	if (this->shape == 0) { //if Sphere
		float distance = (*pos - *(rhs.pos)).Length(); //Calculate the distance
		if (distance <= (width / 2) + (rhs.width / 2)) { //Compare this distance with the sum of their radius
			cp->a = this;
			cp->b = &rhs;
			cp->normal = Vector2(rhs.pos->x - pos->x, rhs.pos->y - pos->y).Normalised();
			cp->penetration = (width / 2) + (rhs.width / 2) - distance;

			counter++;
			return true;
		}
	}
	else { //if Square or box
		float distanceX = abs(pos->x - rhs.pos->x);
		float distanceY = abs(pos->y - rhs.pos->y);
		if (distanceX <= (width / 2 + rhs.width / 2)) {
			if (distanceY <= (height / 2) + (rhs.height / 2)) {
				cp->a = this;
				cp->b = &rhs;
				if (distanceX <= distanceY) {
					cp->normal = Vector2(rhs.pos->x - pos->x, 0).Normalised();
					cp->penetration = (width / 2) + (rhs.width / 2) - distanceX;
				}
				else {
					cp->normal = Vector2(0, rhs.pos->y - pos->y).Normalised();
					cp->penetration = (height / 2) + (rhs.height / 2) - distanceY;
				}

				counter++;
				return true;
			}
		}
	}
	return false;
}

bool CollisionVolume::SphereAABB(CollisionVolume& rhs, CollisionPair* cp) {

	if (!this->collider || !rhs.collider)
		return false;

	float closestX;
	float closestY;
	if (this->shape == 0) { // Sphere collide with AABB
		closestX = Maths::Clamp(pos->x, rhs.pos->x - (rhs.width / 2), rhs.pos->x + (rhs.width / 2));
		closestY = Maths::Clamp(pos->y, rhs.pos->y - (rhs.height / 2), rhs.pos->y + (rhs.height / 2));

		float distance = sqrt(pow(pos->x - closestX, 2) + pow(pos->y - closestY, 2));
		if (distance <= (width / 2)){
			cp->a = this;
			cp->b = &rhs;
			cp->normal = Vector2(rhs.pos->x - pos->x, rhs.pos->y - pos->y).Normalised();
			cp->penetration = (width / 2) - distance;

			counter++;
			return true;
		}
	}
	else { //AABB collide with Sphere
		closestX = Maths::Clamp(rhs.pos->x, pos->x - (width / 2), pos->x + (width / 2));
		closestY = Maths::Clamp(rhs.pos->y, pos->y - (height / 2), pos->y + (height / 2));

		float distance = sqrt(pow(rhs.pos->x - closestX, 2) + pow(rhs.pos->y - closestY, 2));
		if (distance < rhs.width / 2) {
			cp->a = &rhs;
			cp->b = this;
			cp->normal = Vector2(pos->x - rhs.pos->x, pos->y - rhs.pos->y).Normalised();
			cp->penetration = (rhs.width / 2) - distance;

			counter++;
			return true;
		}
	}
	return false;
}