#include "stdafx.h"
#include "VerticalPickDemo.h"
#include "Viewer/Freedom.h"
#include "Environment/Terrain.h"

void VerticalPickDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 0, 0);
	Context::Get()->GetCamera()->Position(44, 51, -49);
	((Freedom *)Context::Get()->GetCamera())->Speed(80, 2);


	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->Pass(0);

	meshShader = new Shader(L"09_Mesh.fx");
	sphere = new MeshRender(shader, new MeshSphere(0.5f));
	sphere->AddTransform();
}

void VerticalPickDemo::Destroy()
{
	
}

void VerticalPickDemo::Update()
{
	Vector3 position;
	sphere->GetTransform(0)->Position(&position);

	if (Keyboard::Get()->Press(VK_UP))
		position.z += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= 20.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 20.0f * Time::Delta();


	Vector3 scale;
	sphere->GetTransform(0)->Scale(&scale);

	position.y = terrain->GetHeight(position) + scale.y * 0.5f;
	sphere->GetTransform(0)->Position(position);
	sphere->UpdateTransforms();

	terrain->Update();
}

void VerticalPickDemo::Render()
{
	terrain->Render();
	sphere->Render();
}