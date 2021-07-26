#include "stdafx.h"
#include "WorldDemo.h"

void WorldDemo::Initialize()
{
	shader = new Shader(L"04_World.fx");

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

	
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));


	D3DXMatrixIdentity(&world[0]);
	D3DXMatrixIdentity(&world[1]);
	D3DXMatrixIdentity(&world[2]);
}

void WorldDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void WorldDemo::Update()
{
		
}

void WorldDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	//World - 0
	{
		static Vector3 position = Vector3(0, 0, 0);
		ImGui::SliderFloat2("Position", position, -10, 10);

		world[0]._41 = position.x;
		world[0]._42 = position.y;

		shader->AsMatrix("World")->SetMatrix(world[0]);


		Color color = Color(1, 0, 0, 1);
		shader->AsVector("Color")->SetFloatVector(color);

		shader->Draw(0, 0, 6);
	}

	//World - 1
	{
		static Vector3 position = Vector3(0, 0, 0);
		ImGui::SliderFloat2("Position1", position, -10, 10);

		world[1]._41 = position.x;
		world[1]._42 = position.y;

		shader->AsMatrix("World")->SetMatrix(world[1]);


		Color color = Color(0, 1, 0, 1);
		shader->AsVector("Color")->SetFloatVector(color);

		shader->Draw(0, 0, 6);
	}

	//World - 2
	{
		static Vector3 position = Vector3(0, 0, 0);
		ImGui::SliderFloat2("Position2", position, -10, 10);

		world[2]._41 = position.x;
		world[2]._42 = position.y;

		world[2]._11 = 2.0f;
		world[2]._22 = 2.0f;

		shader->AsMatrix("World")->SetMatrix(world[2]);


		Color color = Color(0, 0, 1, 1);
		shader->AsVector("Color")->SetFloatVector(color);

		shader->Draw(0, 0, 6);
	}
}