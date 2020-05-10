#pragma once
#include <vector>
#include "BadRobot.h"
#include "Laser.h"

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class GameMap;
		class SimObject;
		class GameSimsRenderer;
		class GameSimsPhysics;
		class TextureBase;
		class TextureManager;
		class PlayerCharacter;	

		class BadFoodGame {
			struct AttackPattern {
				Vector2 startPos;
				float range;
				int cooldown;
			};
		public:
			BadFoodGame();
			~BadFoodGame();

			void Update(float dt);

			void AddNewObject(SimObject* object);

			std::vector<AttackPattern*> atk();

		protected:
			void InitialiseGame();

			TextureManager*		texManager;
			GameSimsRenderer*	renderer;
			GameSimsPhysics*	physics;
			GameMap* currentMap;

			PlayerCharacter* player;

			float gameTime, foodTime, robotTime, coinTime;

			int currentScore;
			int foodCount;
			int coins;
			int balloons;
			int lives;

			std::vector<SimObject*> gameObjects;
			std::vector<SimObject*> newObjects;
			std::vector<BadRobot*> robots;
			std::vector<Laser*> lasers;
			std::vector<AttackPattern*> atkPat;
		};
	}
}

