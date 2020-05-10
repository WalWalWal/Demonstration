#pragma once
#include <string>
#include <vector>

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"

namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsPhysics;
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;

		struct MapInfo {
			bool grass = false;
			bool block = false;
			bool occupied = false; //for robot protect mode
		};

		struct MapNode {
			MapNode* bestParent;
			int posX;
			int posY;
			float g;
			float h;
			bool closed = false;
		};


		class GameMap	{
			friend class BadRobot;
			friend class Food;
		public:
			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics& physics);
			~GameMap();

			void DrawMap(GameSimsRenderer & r);

			int GetMapWidth() const {
				return mapWidth;
			}

			int GetMapHeight() const {
				return mapHeight;
			}

			bool CheckGrass(Vector2 pos);
			Vector2 protectTiles();

			void ClearOccupied();

			bool GeneratePath(Vector2 from, Vector2 to,
				std::vector < Vector2 > & path);

		protected:
			void BuildMapMesh();
			void AddNewTilePos(int x, int y, std::vector<Vector3>& pos);
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex);

			GameSimsPhysics* physics;

			int mapWidth;
			int mapHeight;
			int structureCount;

			Rendering::TextureBase*	tileTexture;
			char*	mapData;
			int*			mapCosts;
			Vector2*		mapTexCoords;

			MeshGeometry* mapMesh;

			MapInfo*					matrix[30][20];
			vector<MapNode*>			openList;
		};
	}
}

