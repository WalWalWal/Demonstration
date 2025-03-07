#include "Laser.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"

#include "../../Common/Maths.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL;
using namespace CSC3222;

Vector4 frames[16] = {
	Vector4(0, 36, 16 , 7), //9 o clock

	Vector4(1, 21, 16, 8),
	Vector4(3, 4, 13, 12),
	Vector4(22, 1, 8 ,15),
	Vector4(37, 1, 7 ,16), //12 0 clock

	Vector4(51,1, 8, 15),
	Vector4(65, 4 , 13 ,12),
	Vector4(65, 21, 15, 8),
	Vector4(65, 36, 16, 7),//3 o clock

	Vector4(65, 50, 15 , 8),
	Vector4(65, 63, 13, 12),
	Vector4(51, 63, 8, 15),
	Vector4(37, 63, 7 ,16), //6 o clock
	Vector4(22, 63 , 8, 15),
	Vector4(3, 63, 13 ,12),
	Vector4(1, 50, 15, 8)
};

Laser::Laser(Vector2 direction) : SimObject()	{
	texture = texManager->GetTexture("bullet.png");

	clear = false;
	id = 'l';

	inverseMass = 1; // Light mass for fast speed
	elasticity = 1; // Won't slow down after collision
	AddForce(direction * 100);
	collider = new CollisionVolume(this, 0, true, &position);
	collider->collision = false;
}

Laser::~Laser()	{

}

void Laser::DrawObject(GameSimsRenderer &r) {
	Vector2 texPos;
	Vector2 texSize = Vector2(16, 16);

	float test = atan2(velocity.y, velocity.x);

	float degrees = Maths::RadiansToDegrees(test);

	degrees += 180.0f;

	if (degrees == 360.0f) {
		degrees = 0;
	}

	degrees /= 360.0f;
	degrees *= 16.0f;

	int frame = (int)degrees;

	texPos.x = frames[frame].x;
	texPos.y = frames[frame].y;

	texSize.x = frames[frame].z;
	texSize.y = frames[frame].w;

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}

bool Laser::UpdateObject(float dt) {
	velocity /= 0.999f;

	time += dt;
	if (time > 1) {
		collider->collision = true;
	}
	
	if (refresh || clear) return false;
	if (position.x < 0 || position.x> 480) return false;
	if (position.y < 0 || position.y> 320) return false;
	return true;
}