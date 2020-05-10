#include "BadRobot.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include <iostream>

using namespace NCL;
using namespace CSC3222;

Vector4 animFramesGreen[] = {
	Vector4(71,142,18,18),
	Vector4(102,142,20,18),
	Vector4(135,142,19,18),
	Vector4(166,142,19,18),
	Vector4(198,140,20,18)
};

Vector4 animFramesWhite[] = {
	Vector4(71,266,17,22),
	Vector4(104,265,19,22),
	Vector4(137,266,17,22),
	Vector4(168,266,17,22),
	Vector4(200,264,17,23)
};

BadRobot::BadRobot(PlayerCharacter& player, GameMap* m,
	std::vector<BadRobot*> robots) : SimObject() {
	this->player = &player;
	map = m;
	this->robots = robots;

	stop = false;

	texture = texManager->GetTexture("TL_Creatures.png");
	animFrameCount	= 4;

	int r = rand() % 2;

	id = 'r';

	if (r == 0) {
		type = RobotType::Green;
	}
	else {
		type = RobotType::White;
	}
	inverseMass = 0.2f;
	collider = new CollisionVolume(this, 1, true, &position);

	state = 3;
}

BadRobot::~BadRobot() {

}

void BadRobot::importAtk(Vector2 pos, float r, int cd) {
	AttackPattern* at = new AttackPattern();
	at->startPos = pos;
	at->range = r;
	at->cooldown = cd;
	atkPat.emplace_back(at);
}

Vector2 BadRobot::laserDir() {
	return Vector2(player->GetPosition()->x - position.x,
		player->GetPosition()->y - position.y).Normalised();
}

Vector2 BadRobot::laserPos() {
	Vector2 dir = laserDir();
	return { dir.x*18.0f + position.x, dir.y * 18.0f + position.y };
}

Vector2 BadRobot::Separation(std::vector<BadRobot*> r) {
	Vector2 dir;
		for (auto i : r) {
		if (i == this) {
			continue;
		}

		float distance = Vector2(this->position - i->position).Length();

		if (distance > 20) {
			continue;
		}

		float strength = 1.0f - (distance / 32);
		dir += (this->position - i->position).Normalised() * strength;
	}
	return dir.Normalised();
}

bool BadRobot::UpdateObject(float dt) {
	UpdateAnimFrame(dt);
	

	if (refresh) return false;

	return true;
}

void BadRobot::DrawObject(GameSimsRenderer& r) {
	Vector2	texPos;
	Vector2 texSize;

	switch (type) {
		case RobotType::Green:{
			texPos	= Vector2(animFramesGreen[currentanimFrame].x, animFramesGreen[currentanimFrame].y);
			texSize = Vector2(animFramesGreen[currentanimFrame].z, animFramesGreen[currentanimFrame].w);
		}break;
		case RobotType::White:{
			texPos	= Vector2(animFramesWhite[currentanimFrame].x, animFramesWhite[currentanimFrame].y);
			texSize = Vector2(animFramesWhite[currentanimFrame].z, animFramesWhite[currentanimFrame].w);
		}break;
	}

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, false);
}