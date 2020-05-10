#pragma once
#include <vector>
#include <iostream>
#include "GameMap.h"
#include "../../Common/Vector2.h"
#include "../../Common/Maths.h"

namespace NCL {
	namespace CSC3222 {

		using namespace Maths;
		using namespace std;

		class RigidBody;
		class CollisionVolume;
		class GameMap;

		struct CollisionPair {
			CollisionVolume* a;
			CollisionVolume* b;
			Vector2 normal;
			float penetration;
		};

		struct CollisionBody {
			RigidBody* rbA;
			RigidBody* rbB;
		};

		class GameSimsPhysics	{
		public:
			GameSimsPhysics();
			~GameSimsPhysics();

			void Update(float dt);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void importMap(GameMap* m);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);

			bool death;

		protected:
			GameMap* map;

			void Integration(float dt, bool c);
			bool CollisionDetection();

			vector<CollisionPair*>		collisionPairs;
			vector<CollisionBody*>		collisionBodys;
			vector<RigidBody*>			staticRigid;
			vector<RigidBody*>			dynamicRigid;
			vector<CollisionVolume*>	staticColliders;
			vector<CollisionVolume*>	dynamicColliders;
		};
	}
}

