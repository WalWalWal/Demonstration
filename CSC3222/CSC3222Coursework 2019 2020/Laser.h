#pragma once
#include "SimObject.h"

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class Laser : public SimObject {
		public:
			Laser(Vector2 direction);
			~Laser();

			bool refresh = false;

			void DrawObject(GameSimsRenderer &r) override;
			bool UpdateObject(float dt) override;

		protected:
			float time = 0;
		};
	}
}

