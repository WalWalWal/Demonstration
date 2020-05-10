#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include <iostream>

using namespace std;

using namespace NCL;
using namespace CSC3222;

GameSimsPhysics::GameSimsPhysics()	{
}

GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::importMap(GameMap* m) {
	map = m;
}

bool comparator(CollisionVolume* lhs, CollisionVolume* rhs) {
	return (lhs->getMin() < rhs->getMin());
}

void GameSimsPhysics::Update(float dt) {
	death = false;
	sort(dynamicColliders.begin(), dynamicColliders.end(), &comparator);
	collisionPairs.clear(); //Renew every frame
	Integration(dt, CollisionDetection());
}

void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	if (b->GetMass() == 0) staticRigid.emplace_back(b);
	else dynamicRigid.emplace_back(b);
}

void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	if (b->GetMass() == 0) {
		auto at = std::find(staticRigid.begin(), staticRigid.end(), b);

		if (at != staticRigid.end()) {
			//maybe delete too?
			staticRigid.erase(at);
		}
	}
	else {
		auto at = std::find(dynamicRigid.begin(), dynamicRigid.end(), b);

		if (at != dynamicRigid.end()) {
			//maybe delete too?
			dynamicRigid.erase(at);
		}
	}
}

void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	if (c->dynamic) {
		dynamicColliders.emplace_back(c);
	}
	else {
		staticColliders.emplace_back(c);
		sort(staticColliders.begin(), staticColliders.end(), &comparator);
	}
}


void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	if(c->dynamic) {
		auto at = std::find(dynamicColliders.begin(), dynamicColliders.end(), c);

		if (at != dynamicColliders.end()) {
			//maybe delete too?
			dynamicColliders.erase(at);
		}
	}
	else {
		auto at = std::find(staticColliders.begin(), staticColliders.end(), c);

		if (at != staticColliders.end()) {
			//maybe delete too?
			staticColliders.erase(at);
		}
	}
	
}

void GameSimsPhysics::Integration(float dt, bool c) {
	RigidBody* rbA = new RigidBody;
	RigidBody* rbB = new RigidBody;
	bool found = false;

	for (vector<RigidBody*>::iterator it1 = dynamicRigid.begin(); it1 != dynamicRigid.end(); ++it1) {
		RigidBody* rb = *it1;
		rb->velocity += rb->force * rb->inverseMass * dt;

		if (map->CheckGrass(*rb->GetPosition())) {
			if (rb->id == 'p')
				rb->velocity *= 0.7f; //30% speed penalty
			if (rb->id == 'r')
				rb->velocity *= 0.4f; //60% speed penalty
		}
	}

	if (c) {
		for (vector<CollisionPair*>::iterator pair = collisionPairs.begin(); pair != collisionPairs.end(); ++pair) {
			CollisionPair* cp = *pair;

			rbA = cp->a->rb;
			rbB = cp->b->rb;

			if ((cp->a->rb->id == 'l' && cp->b->rb->id == 'p')) {
				death = true;
			}
				

			if ((cp->a->rb->id == 'p' && cp->b->rb->id == 'l')) {
				death = true;
			}

			if (death) {
				for (auto rb : staticRigid) { //delete coins and foods
					if (rb->id == 'f') {
						rb->clear = true;
					}
				}
				break;
			}
				

			if ((cp->a->rb->id == 'l' && cp->b->rb->id == 'r')) {
				rbA->clear = true;
				rbB->stop = true;
			}
			if ((cp->a->rb->id == 'r' && cp->b->rb->id == 'l')) {
				rbB->clear = true;
				rbA->stop = true;
			}

			if (rbA->id != rbB->id) {
				rbA->collision++; //Collision counter for event to occur
				rbA->position -= cp->normal * cp->penetration *  //projection
					rbA->inverseMass / (rbA->inverseMass + rbB->inverseMass);

				rbB->collision++; //Collision counter for event to occur
				rbB->position += cp->normal * cp->penetration *  //projection
					rbB->inverseMass / (rbA->inverseMass + rbB->inverseMass);

				rbA->AddImpulse(rbB, cp);

				Vector2 resting = Vector2({ cp->penetration , cp->penetration });
			}	
		}
	}

	for (vector<RigidBody*>::iterator it1 = dynamicRigid.begin(); it1 != dynamicRigid.end(); ++it1) {
		RigidBody* rb = *it1;
		rb->position += rb->velocity * dt;
		rb->velocity *= 0.999f;
	}
}

bool GameSimsPhysics::CollisionDetection() {
	for (vector<CollisionVolume*>::iterator it1 = dynamicColliders.begin(); it1 != dynamicColliders.end(); ++it1) {
		CollisionVolume* i = *it1;
		i->collision = false;
		for (vector<CollisionVolume*>::iterator it2 = it1 + 1; it2 != dynamicColliders.end(); ++it2) {
			CollisionVolume* j = *it2;
			if (i != j) {
				if (i->getMax() < j->getMin()) break;
				//Compare the x axis extent

				CollisionPair* cp = new CollisionPair();
				if (i->shape == j->shape) //if Sphere & Sphere or AABB & AABB
				{
					if (i->SameShape(*j, cp)) collisionPairs.push_back(cp);
				}  //if collision occur
				else if (i->SphereAABB(*j, cp)) collisionPairs.push_back(cp);
				//if Sphere & AABB, if collision occur	
			}
		}

		for (vector<CollisionVolume*>::iterator it2 = staticColliders.begin(); it2 != staticColliders.end(); ++it2) {
			CollisionVolume* j = *it2;
			CollisionPair* cp = new CollisionPair();
			if (i->getMin() > j->getMax()) continue;
			//skip the objects on left side
			if (i->getMax() < j->getMin()) break;
			//ignore the objects on right side
			
			if (i->shape == j->shape) //if Sphere & Sphere or AABB & AABB
			{
				if (i->SameShape(*j, cp)) collisionPairs.push_back(cp);
			}  //if collision occur
			else if (i->SphereAABB(*j, cp)) collisionPairs.push_back(cp);
			//if Sphere & AABB, if collision occur	
		}
	}
	if (collisionPairs.size() > 0)
	{
		return true;
	} else return false;
}