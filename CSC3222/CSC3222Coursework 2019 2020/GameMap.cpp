#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>

using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

Vector4 buildingTypes[4] = {
	Vector4(320,16,64,80),	//tree
	Vector4(16,16,96,80), //blue building
	Vector4(128,32,64,64), //red building
	Vector4(208,32,96,64)  //green building
};

GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics& physics)	{
	tileTexture = texManager.GetTexture("badfoodTiles.png");

	this->physics = &physics;

	srand(time(NULL));

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	for (auto& i : matrix)
		for (auto& n : i)
			n = new MapInfo();

	mapData		= new char[mapWidth * mapHeight];
	mapCosts	= new int[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';
		}
	}

	BuildMapMesh();
}

GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;

	delete mapMesh;
}

void GameMap::DrawMap(GameSimsRenderer & r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
}

bool GameMap::CheckGrass(Vector2 pos) {
	int a = round(pos.x / 16.0f);
	int b = round(pos.y / 16.0f);
	bool grass = matrix[a][b]->grass;
	return grass;
}

Vector2 GameMap::protectTiles() {
	vector<Vector2> protect;
	for (int a = 20; a < 30; a++) {
		for (int b = 1; b < 11; b++) {
			if (!matrix[a][b]->block && !matrix[a][b]->occupied) {
				Vector2 t = Vector2((float)(a + 0.5f) * 16, (float)b *16);
				protect.push_back(t);
			}
		}
	}
	cout << protect.size();
	
	int i = rand() % protect.size();
	int a = (int)(protect[i].x - 8) / 16;
	int b = (int)protect[i].y / 16;
	matrix[a][b]->occupied = true;
	return protect[i];
}

void GameMap::ClearOccupied() {
	for (int a = 20; a < 30; a++) {
		for (int b = 1; b < 11; b++) {
			matrix[a][b]->occupied = false;
		}
	}
}


void GameMap::BuildMapMesh() {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 flatGrassTile = Vector2(288, 144);
	Vector2 primaryTile;
	Vector2 secondaryTile = flatGrassTile; //some sit on top of another tile type, usually grass!

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
			bool doSecondary = false;
			bool collision = false;

			switch (tileType) {
				//the normal tile types first
				case 'A':primaryTile = Vector2(272,112);break; //top left grass
				case 'B':primaryTile = Vector2(288, 112); break;//top grass
				case 'C':primaryTile = Vector2(320, 112); break;//top right grass
				case 'D':primaryTile = Vector2(272, 128); break;//left grass
				case 'E':primaryTile = Vector2(320, 128); break;//right grass
				case 'F':primaryTile = Vector2(272, 160); break;//bottom left
				case 'G':primaryTile = Vector2(320, 160); break;//bottom right
				case 'H':primaryTile = Vector2(288, 160); break;//bottom grass

				case 'I':primaryTile = Vector2(336, 112); break; //top left grass
				case 'J':primaryTile = Vector2(352, 112); break; //top right grass
				case 'K':primaryTile = Vector2(336, 128); break; //bottom left grass
				case 'L':primaryTile = Vector2(352, 128); break; //bottom right grass

				case 'M':primaryTile = Vector2(208, 112); doSecondary = true; break; //top left fence
				case 'N':primaryTile = Vector2(224, 112); doSecondary = true; break; //top fence
				case 'O':primaryTile = Vector2(240, 112); doSecondary = true; break; //top right fence
				case 'P':primaryTile = Vector2(208, 128); doSecondary = true; break; //left fence
				case 'Q':primaryTile = Vector2(208, 144); doSecondary = true; break; //bottom left fence
				case 'R':primaryTile = Vector2(240, 144); doSecondary = true; break; //bottom right fence
//Stone wall bits
				case 'a':primaryTile = Vector2(16, 112); doSecondary = true; break; //top left grass
				case 'b':primaryTile = Vector2(32, 112); break;//Horizontal A
				case 'c':primaryTile = Vector2(32, 128); break;//Horizontal B
				case 'd':primaryTile = Vector2(144, 112); doSecondary = true; break;//top right grass
				case 'e':primaryTile = Vector2(144, 128); break;//Side
				case 'f':primaryTile = Vector2(16, 160); break;//bottom left A
				case 'g':primaryTile = Vector2(16, 176); doSecondary = true; break;//bottom left B
	
				case 'h':primaryTile = Vector2(64, 160); doSecondary = true; break;//right hand corner A
				case 'i':primaryTile = Vector2(64, 176); doSecondary = true; break;//right hand corner B

				case 'j':primaryTile = Vector2(96, 160); doSecondary = true; break;//Left hand corner A
				case 'k':primaryTile = Vector2(96, 176); doSecondary = true; break;//Left hand corner B

				case 'l':primaryTile = Vector2(144, 160); break;//Bottom right A
				case 'm':primaryTile = Vector2(144, 176); doSecondary = true;  break;//Bottom right B
				case 'n':primaryTile = Vector2(32, 176); doSecondary = true; break;//bottom horizontal

				case 'o':primaryTile = Vector2(80, 176); doSecondary = true; break;//Stone to grass transition
				case 'p':primaryTile = Vector2(80, 176); doSecondary = true; break;//stone to road transition

				case '1':primaryTile = Vector2(352, 144); break; //Road Tile
				case '2':primaryTile = Vector2(64, 144);  break;//Stone Tile
				case '3':primaryTile = Vector2(16, 192); doSecondary  = true; break; //Flower A
				case '4':primaryTile = Vector2(32, 192); doSecondary  = true; break;//Flower B
				case '5':primaryTile = Vector2(48, 192); doSecondary  = true; break;//Flower C
				case '6':primaryTile = Vector2(64, 192); doSecondary  = true; break;//Flower D
				case '7':primaryTile = Vector2(176, 176); doSecondary = true; break;//Sign A
				case '8':primaryTile = Vector2(192, 176); doSecondary = true; break;//Sign B
				case '9':primaryTile = Vector2(208, 176); doSecondary = true; break;//Sign C	
				case '0': {
					static Vector2 grassTiles[] = {
						Vector2(288,128),	//	
						Vector2(304,144),	//
						Vector2(304,128),	//
						Vector2(288,144),
					};
					primaryTile = grassTiles[Maths::Clamp(rand() % 20, 0, 3)];
				}break;//Random grass tiles
			}			
			if (doSecondary) {
				AddNewTilePos(x, y, positions);
				AddNewTileTex((int)secondaryTile.x, (int)secondaryTile.y, texCoords);
				secondaryTile = flatGrassTile;
			}

			AddNewTilePos(x, y, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}

	
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos) {
	Vector3 topLeft		= Vector3((x + 0) * 16.0f, (y + 0) * 16.0f, 0);
	Vector3 topRight	= Vector3((x + 1) * 16.0f, (y + 0) * 16.0f, 0);
	Vector3 bottomLeft	= Vector3((x + 0) * 16.0f, (y + 1) * 16.0f, 0);
	Vector3 bottomRight = Vector3((x + 1) * 16.0f, (y + 1) * 16.0f, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex) {
	Vector2 topLeft		= Vector2((x + 0.0f) , (y + 0.0f));
	Vector2 topRight	= Vector2((x + 16.0f), (y + 0.0f));
	Vector2 bottomLeft	= Vector2((x + 0.0f) , (y + 16.0f));
	Vector2 bottomRight = Vector2((x + 16.0f), (y + 16.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft); 
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}

bool GameMap::GeneratePath(Vector2 from, Vector2 to,
	std::vector < Vector2 >& path){

	bool found = false;
	int count = 0;
	openList.clear();
	path.clear();

	int startX = floor(from.x / 16.0f);
	int startY = floor(from.y / 16.0f);
	int endX = floor(to.x / 16.0f);
	int endY = floor(to.y / 16.0f);

	MapNode* startNode = new MapNode();
	startNode->posX = startX;
	startNode->posY = startY;
	startNode->g = 0;
	startNode->h = abs(endX - startX) + abs(endY - startY);
	openList.emplace_back(startNode);

	float bestF = openList[0]->g + openList[0]->h;
	MapNode* bestNode = openList[0];

	while (!found) {
		for (auto& i : openList) {
			if (!i->closed) {
				bestNode = i;
				bestF = i->g + i->h;
				break;
			}
		}
		for (auto& i : openList) {
			if (i->closed) continue;
			float iF = i->g + i->h;
			if (iF < bestF) {
				bestF = iF;
				bestNode = i;
			}
			else if (iF == bestF) {
				if (abs(endX - i->posX) < abs(endX - bestNode->posX)) {
					bestF = iF;
					bestNode = i;
				}
			}
		}

		int x = bestNode->posX;
		int y = bestNode->posY;
		bool left = true;
		bool right = true; 
		bool up = true;
		bool down = true;
		
		for (auto& it : openList) {
			MapNode* testNode = it;

			if (x == 0 || matrix[x - 1][y]->block
				|| matrix[x - 1][y]->occupied) left = false;
			else if (testNode->posX == x - 1 && testNode->posY == y) {
				if (testNode->bestParent != bestNode) {
					float cost = abs(testNode->posX - bestNode->posX)
						+ abs(testNode->posY - bestNode->posY);
					if (testNode->g > bestNode->g + cost)
					testNode->g = bestNode->g + cost;
				}
				left = false;
			}

			if (x == mapWidth || matrix[x + 1][y]->block
				|| matrix[x + 1][y]->occupied) right = false;
			else if (testNode->posX == x + 1 && testNode->posY == y) {
				if (testNode->bestParent != bestNode) {
					float cost = abs(testNode->posX - bestNode->posX)
						+ abs(testNode->posY - bestNode->posY);
					if (testNode->g > bestNode->g + cost)
					testNode->g = bestNode->g + cost;
				}
				right = false;
			}
				

			if (y == 0 || matrix[x][y - 1]->block
				|| matrix[x][y - 1]->occupied) up = false;
			else if (testNode->posX == x && testNode->posY == y - 1){
				if (testNode->bestParent != bestNode) {
					float cost = abs(testNode->posX - bestNode->posX)
						+ abs(testNode->posY - bestNode->posY);
					if (testNode->g > bestNode->g + cost)
					testNode->g = bestNode->g + cost;
				}
				up = false;
			}

			if (y == mapHeight || matrix[x][y + 1]->block
				|| matrix[x][y + 1]->occupied) down = false;
			else if (testNode->posX == x && testNode->posY == y + 1) {
				if (testNode->bestParent != bestNode) {
					float cost = abs(testNode->posX - bestNode->posX)
						+ abs(testNode->posY - bestNode->posY);
					if (testNode->g > bestNode->g + cost)
					testNode->g = bestNode->g + cost;
				}
				down = false;
			}		
		}

		if (left) {
			MapNode* newNode = new MapNode();
			newNode->bestParent = bestNode;
			newNode->posX = x-1;
			newNode->posY = y;

			float cost = abs(newNode->posX - bestNode->posX) 
				+ abs(newNode->posY - bestNode->posY);
			newNode->g = bestNode->g + cost;

			newNode->h = abs(endX - newNode->posX) + abs(endY - newNode->posY);
			openList.emplace_back(newNode);
		}

		if (right) {
			MapNode* newNode = new MapNode();
			newNode->bestParent = bestNode;
			newNode->posX = x + 1;
			newNode->posY = y;

			float cost = abs(newNode->posX - bestNode->posX)
				+ abs(newNode->posY - bestNode->posY);
			newNode->g = bestNode->g + cost;

			newNode->h = abs(endX - newNode->posX) + abs(endY - newNode->posY);
			openList.emplace_back(newNode);
		}

		if (up) {
			MapNode* newNode = new MapNode();
			newNode->bestParent = bestNode;
			newNode->posX = x;
			newNode->posY = y - 1;
			
			float cost = abs(newNode->posX - bestNode->posX)
				+ abs(newNode->posY - bestNode->posY);
			newNode->g = bestNode->g + cost;

			newNode->h = abs(endX - newNode->posX) + abs(endY - newNode->posY);
			openList.emplace_back(newNode);
		}

		if (down) {
			MapNode* newNode = new MapNode();
			newNode->bestParent = bestNode;
			newNode->posX = x;
			newNode->posY = y + 1;
			
			float cost = abs(newNode->posX - bestNode->posX)
				+ abs(newNode->posY - bestNode->posY);
			newNode->g = bestNode->g + cost;

			newNode->h = abs(endX - newNode->posX) + abs(endY - newNode->posY);
			openList.emplace_back(newNode);
		}

		// Prevent 
		if (abs(endX - bestNode->posX) == 1 && abs(endY - bestNode->posY) == 0) {
			MapNode* newNode = new MapNode();
			newNode->bestParent = bestNode;
			newNode->posX = endX;
			newNode->posY = endY;
			bestNode = newNode;
			found = true;
		}

		if (abs(endX - bestNode->posX) == 0 && abs(endY - bestNode->posY) == 1) {
			MapNode* newNode = new MapNode();
			newNode->bestParent = bestNode;
			newNode->posX = endX;
			newNode->posY = endY;
			bestNode = newNode;
			found = true;
		}
			
		count++;
		if (count > 200) {
			return false;
		}
		bestNode->closed = true;
	}

	if (found) {
		MapNode* findNode = bestNode;
		while (findNode != openList[0]) {
			Vector2 node = { (findNode->posX + 0.5f) * 16.0f, findNode->posY * 16.0f };
			path.push_back(node);
			findNode = findNode->bestParent;
		}
		reverse(path.begin(), path.end());
	}
	return false;
}