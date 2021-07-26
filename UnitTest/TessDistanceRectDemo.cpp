#include "stdafx.h"
#include "TessDistanceRectDemo.h"

void TessDistanceRectDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(0, 0, -5);
	((Freedom *)Context::Get()->GetCamera())->Speed(10, 0);


	shader = new Shader(L"46_DistanceRect.fxo");

	perFrame = new PerFrame(shader);
	transform = new Transform(shader);


	Vertex vertices[4];
	vertices[0].Position = Vector3(-10.0f, -10.0f, 0);
	vertices[1].Position = Vector3(-10.0f, +10.0f, 0);
	vertices[2].Position = Vector3(+10.0f, -10.0f, 0);
	vertices[3].Position = Vector3(+10.0f, +10.0f, 0);

	vertexBuffer = new VertexBuffer(vertices, 4, sizeof(Vertex));
}

void TessDistanceRectDemo::Update()
{
	static Vector2 distance = Vector2(1, 100);
	ImGui::SliderFloat2("Distance", distance, 0, 1000);

	shader->AsVector("Distance")->SetFloatVector(distance);

	transform->Update();
	perFrame->Update();
}

void TessDistanceRectDemo::Render()
{
	transform->Render();
	perFrame->Render();

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	shader->Draw(0, 0, 4);
}