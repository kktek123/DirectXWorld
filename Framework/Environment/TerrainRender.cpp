#include "Framework.h"
#include "TerrainRender.h"


TerrainRender::TerrainRender(Shader * shader, Terrain * mesh)
	: mesh(mesh)
{
	Pass(0);
	//mesh->SetShader(shader);

	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		D3DXMatrixIdentity(&worlds[i]);

	instanceBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);	
}

TerrainRender::~TerrainRender()
{
	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceBuffer);
	SafeDelete(mesh);
}

void TerrainRender::Update()
{
	mesh->Update();
}

void TerrainRender::Render()
{
	instanceBuffer->Render();

	mesh->Render();
}

void TerrainRender::EraseTransform()
{
	transforms.erase(transforms.begin());
}

Transform* TerrainRender::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);
	mesh->SetTransform(transform);
	return transform;
}

void TerrainRender::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);
}
