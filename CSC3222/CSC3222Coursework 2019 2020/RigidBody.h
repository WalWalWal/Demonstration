#pragma once
#include "../../Common/Vector2.h"
#include "GameSimsPhysics.h"
#include <iostream>

using namespace std;

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsPhysics;
		class RigidBody{
			friend class GameSimsPhysics;
			friend class CollisionVolume;
		public:
			RigidBody();
			~RigidBody();

			char id;

			Vector2* GetPosition() {
				return &position;
			}

			void SetPosition(const Vector2& newPosition) {
				position = newPosition;
			}

			Vector2 GetVelocity() const {
				return velocity;
			}

			void SetVelocity(const Vector2& newVelocity) {
				velocity = newVelocity;
			}

			float GetMass() { return inverseMass; }

			void AddForce(const Vector2& newForce) {
				force += newForce;
			}

			void ClearForce() {
				force = { 0,0 };
				velocity = { 0,0 };
			}

			void AddImpulse(RigidBody* r, CollisionPair* cp) {
				force -= force; //Clear the force

				float J =  -(1 + elasticity) * Vector2::Dot(velocity - r->velocity , cp->normal)
					/ (inverseMass + r->inverseMass);
				
				velocity += cp->normal * J * inverseMass;
				r->velocity -= cp->normal * J * r->inverseMass;
			}


		protected:
			Vector2 position;
			Vector2 velocity;
			Vector2 force;

			bool stop, clear;

			int	collision; // indicate there is/are collision for the event to run

			float inverseMass;
			float elasticity;
		};
	}
}
