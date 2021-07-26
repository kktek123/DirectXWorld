#include "stdafx.h"
#include "RaycastDemo.h"
#include "Viewer/Freedom.h"
#include "Environment/Terrain.h"

void RaycastDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 0, 0);
	Context::Get()->GetCamera()->Position(44, 51, -49);
	((Freedom *)Context::Get()->GetCamera())->Speed(80, 2);


	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->Pass(1);

	meshShader = new Shader(L"09_Mesh.fx");
	sphere = new MeshRender(shader, new MeshSphere(0.5f));
	sphere->AddTransform();
}

void RaycastDemo::Destroy()
{
	SafeDelete(terrain);
}

void RaycastDemo::Update()
{
	Vector3 scale;
	sphere->GetTransform(0)->Scale(&scale);

	Vector3 position = terrain->GetPickedPosition();
	
	string str = to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
	Gui::Get()->RenderText(Vector2(10, 60), Color(1, 0, 0, 1), str);

	
	position.y += scale.y * 0.5f;
	sphere->GetTransform(0)->Position(position);
	sphere->UpdateTransforms();

	terrain->Update();
}

void RaycastDemo::Render()
{
	terrain->Render();
	sphere->Render();
}