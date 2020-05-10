#include "BadFoodGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "BadRobot.h"
#include "Laser.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

BadFoodGame::BadFoodGame()	{
	renderer	= new GameSimsRenderer();
	texManager	= new TextureManager();
	physics		= new GameSimsPhysics();
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

BadFoodGame::~BadFoodGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

// to calculate Fibonacci sequence
int fib(int n)
{
	if (n <= 1)
		return n;
	return fib(n - 1) + fib(n - 2);
}

void BadFoodGame::Update(float dt) {
	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;
	foodTime += dt;
	robotTime += dt;
	coinTime += dt;

	player->death = false;

	renderer->Update(dt);
	physics->Update(dt);
	currentMap->DrawMap(*renderer);

	srand((int)(gameTime * 1000.0f));



	for (auto r : robots) {
		if (r->shoot) {
			Laser* testLaser = new Laser(r->laserDir());
			testLaser->SetPosition(r->laserPos());
			AddNewObject(testLaser);
			lasers.emplace_back(testLaser);
		}
	}

	if (physics->death)
	{
		player->death = true;
	}

	if (player->death) {
		player->SetPosition(Vector2(48, 112));
		lives--;
		foodCount = 0;
		for (auto r : robots) {
			r->refresh = true;
		}

		for (auto l : lasers) {
			l->refresh = true;
		}
		lasers.clear();
		robots.clear();
	}

	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		if (!(*i)->UpdateObject(dt)) { //object has said its finished with
			if ((*i)->GetCollider())
				physics->RemoveCollider((*i)->GetCollider());
			physics->RemoveRigidBody(*i);
			delete (*i);
			i = gameObjects.erase(i);
			break;
		}
		else {
			(*i)->DrawObject(*renderer);
			++i;
		}
	}	

	renderer->DrawString("Score: " + std::to_string(currentScore), Vector2(10, 10));
	renderer->DrawString("Lives left: " + std::to_string(lives), Vector2(10, 30));
	renderer->DrawString("Food: " + std::to_string(foodCount), Vector2(10, 50));
	renderer->DrawString("Coins: " + std::to_string(coins), Vector2(10, 70));
	renderer->DrawString("Balloons: " + std::to_string(balloons), Vector2(10, 90));


	/*

	Some examples of debug rendering!

	
	renderer->DrawBox(Vector2(16,16), Vector2(8, 8), Vector4(1, 0, 0, 1));
	renderer->DrawLine(Vector2(16, 16), Vector2(192, 192), Vector4(1, 1, 0, 1));
	renderer->DrawCircle(Vector2(100, 100), 10.0f, Vector4(1, 0, 1, 1));
	*/
	renderer->Render();

	if (player->death && lives == 0) {
		physics = new GameSimsPhysics();
		InitialiseGame();
	}
}

void BadFoodGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	currentMap = new GameMap("BadFoodMap.txt", gameObjects, *texManager, *physics);
	physics->importMap(currentMap);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter();
	player->SetPosition(Vector2(240, 240));
	AddNewObject(player);

	AttackPattern* atk = new AttackPattern();
	atk->startPos = Vector2(480, 0);
	atk->range = 75.0f;
	atk->cooldown = 4;
	atkPat.emplace_back(atk);

	atk = new AttackPattern();
	atk->startPos = Vector2(0, 320);
	atk->range = 100.0f;
	atk->cooldown = 5;
	atkPat.emplace_back(atk);

	atk = new AttackPattern();
	atk->startPos = Vector2(0, 0);
	atk->range = 50.0f;
	atk->cooldown = 3;
	atkPat.emplace_back(atk);

	atk = new AttackPattern();
	atk->startPos = Vector2(480, 320);
	atk->range = 150.0f;
	atk->cooldown = 7;
	atkPat.emplace_back(atk);

	atk = new AttackPattern();
	atk->startPos = Vector2(240, 160);
	atk->range = 120.0f;
	atk->cooldown = 6;
	atkPat.emplace_back(atk);

	BadRobot* testRobot = new BadRobot(*player, currentMap, robots);
	testRobot->SetPosition({ 192, 108 });
	for (int n = 0; n < 3; n++) {
		int a = rand() % 5;
		testRobot->importAtk(atkPat[a]->startPos, atkPat[a]->range, atkPat[a]->cooldown);
	}
	robots.emplace_back(testRobot);
	AddNewObject(testRobot);

	testRobot = new BadRobot(*player, currentMap, robots);
	testRobot->SetPosition({ 240, 108 });
	for (int n = 0; n < 3; n++) {
		int a = rand() % 5;
		testRobot->importAtk(atkPat[a]->startPos, atkPat[a]->range, atkPat[a]->cooldown);
	}
	robots.emplace_back(testRobot);
	AddNewObject(testRobot);

	testRobot = new BadRobot(*player, currentMap, robots);
	testRobot->SetPosition({ 288, 108 });
	for (int n = 0; n < 3; n++) {
		int a = rand() % 5;
		testRobot->importAtk(atkPat[a]->startPos, atkPat[a]->range, atkPat[a]->cooldown);
	}
	robots.emplace_back(testRobot);
	AddNewObject(testRobot);

	gameTime		= 0;
	currentScore	= 0;
	foodCount		= 0;
	coins			= 0;
	balloons		= 0;
	lives			= 3;
}

void BadFoodGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}