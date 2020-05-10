#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/Maths.h"
#include "RigidBody.h"
#include "GameSimsPhysics.h"

namespace NCL {
	namespace CSC3222 {
		class RigidBody;

		class CollisionVolume {
			friend class GameSimsPhysics;
		public:
			CollisionVolume();
			CollisionVolume(RigidBody* rb, int shape, bool dyna, Vector2* position); //for standard size object 32x32
			CollisionVolume(RigidBody* rb, float w, float h, int shape, bool dyna, Vector2* position); //for special size object
			~CollisionVolume();

			float getMax();
			float getMin();

			bool collider = true;
			bool collision;
			int counter;
			char id;

			bool SameShape(CollisionVolume& rhs);
			bool SameShape(CollisionVolume& rhs, CollisionPair* cp);
			bool SphereAABB(CollisionVolume& rhs, CollisionPair* cp);

		protected:
			float width = 15;
			float height = 15;
			int shape; //0 = shpere, 1 = box

			bool dynamic;

			Vector2* pos;

			RigidBody* rb;
		};
	}
}

