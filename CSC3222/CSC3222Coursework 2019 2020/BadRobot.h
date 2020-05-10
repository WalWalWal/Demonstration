#pragma once
#include "SimObject.h"
#include "GameMap.h"
#include "PlayerCharacter.h"
#include "../../Common/Maths.h"

namespace NCL {
	namespace CSC3222 {
		struct AttackPattern {
			Vector2 startPos;
			float range;
			int cooldown;
		};
		class GameMap;
		class PlayerCharacter;
		class BadRobot : public SimObject
		{
		public:
			BadRobot(PlayerCharacter& player, GameMap* m,
				std::vector<BadRobot*> r);
			~BadRobot();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;

			bool shoot;
			bool refresh = false;

			void importAtk(Vector2 pos, float r, int cd);

			Vector2 laserDir();
			Vector2 laserPos();

			Vector2 Separation(std::vector<BadRobot*> r);

			//State
			void patrol(float dt);
			void attack(float dt);

		protected:
			GameMap* map;

			enum class RobotType {
				Green,
				White,
			};
			RobotType type;

			PlayerCharacter* player;
			Vector2 targetPos;

			std::vector < Vector2 > path;

			int i = 0;
			int state = 1;
			int* foods;

			float shootTime = 0;
			float stopTime = 0;
			std::vector<BadRobot*> robots;
			std::vector<AttackPattern*> atkPat;
		};
	}
}

