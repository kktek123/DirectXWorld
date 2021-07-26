#pragma once
#include "Systems/IExecute.h"

class CullingDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	void CreateMeshData();

private:
	Material* red;
	Shader* shader;
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	PerFrame* perFrame;

	vector<Transform *> transforms;


	Material* floor;
	Shader* gridShader;
	MeshRender* grid;


	float fov = 0.25f;
	float zFar = 10000.0f;

	class Fixity* camera;
	Perspective* perspective;
	Frustum* frustum;

	Shader* modelShader;
	ModelRender* model;
};