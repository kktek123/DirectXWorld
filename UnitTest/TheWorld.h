#pragma once
#include "Systems/IExecute.h"

class TheWorld : public IExecute
{
	virtual void Initialize() override;
	virtual void Ready() {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender();
	virtual void Render() override;
	virtual void PostRender() {};
	virtual void ResizeScreen() {};

private:
	void Mesh();
	void Airplane();
	void Kachujin();
	void Pass(UINT mesh, UINT model, UINT anim);

	Shader* terrainShader;
	Shader* shader;
	class Sky* sky;

	class TerrainLOD* terrainlod;
	class Terrain* terrain;
	class TerrainRender* Rterrain;
	class Shadow* shadow;
	class LakeWater* lakeWater;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* sphere;
	MeshRender* cylinder;
	MeshRender* cube;
	MeshRender* grid;


	ModelRender* airplane = NULL;

	Model* weapon;
	ModelAnimator* kachujin = NULL;

	vector<MeshRender *> meshes;
	vector<ModelRender *> models;
	vector<ModelAnimator *> animators;
};
