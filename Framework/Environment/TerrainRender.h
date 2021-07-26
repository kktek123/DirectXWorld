#pragma once
#include "Terrain.h"
#define MAX_MESH_INSTANCE 600*600

class TerrainRender
{
public:
	TerrainRender(Shader* shader, class Terrain* mesh);
	~TerrainRender();

	Terrain* GetTerrain() { return mesh; }

	void Update();
	void Render();

	void Pass(UINT val) { mesh->Pass(val); }

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();
	void EraseTransform();
private:
	Terrain* mesh;

	vector<Transform *> transforms;
	Matrix worlds[MAX_MESH_INSTANCE];

	VertexBuffer* instanceBuffer;
};