#include "stdafx.h"
#include "ColliderDemo.h"

void ColliderDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0, 0);
	Context::Get()->GetCamera()->Position(0, 3, -8);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 0);


	shader = new Shader(L"15_Mesh.fx");

	gridMaterial = new Material(shader);
	gridMaterial->DiffuseMap(L"White.png");

	grid = new MeshRender(shader, new MeshGrid());
	grid->AddTransform();
	grid->UpdateTransforms();


	t = new Transform();
	t->Position(0.0f, 0.5f, 0);
	t->Scale(2, 1, 1);
	collider = new Collider(t);

	t2 = new Transform();
	t2->Position(2.0f, 0.5f, 0);
	collider2 = new Collider(t2);
}

void ColliderDemo::Destroy()
{
	
}

void ColliderDemo::Update()
{
	grid->Update();


	Vector3 position2;
	t2->Position(&position2);

	if (Keyboard::Get()->Press(VK_RIGHT))
		position2.x += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position2.x -= 3.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position2.z += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position2.z -= 3.0f * Time::Delta();

	t2->Position(position2);


	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 rotation2 = Vector3(0, 0, 0);

	t->RotationDegree(&rotation);
	t2->RotationDegree(&rotation2);

	ImGui::SliderFloat3("Rotation", (float *)rotation, -179, 179);
	ImGui::SliderFloat3("Rotation2", (float *)rotation2, -179, 179);

	t->RotationDegree(rotation);
	t2->RotationDegree(rotation2);


	collider->Update();
	collider2->Update();
}

void ColliderDemo::Render()
{
	gridMaterial->Render();
	grid->Render();


	bool bCheck = collider->IsIntersect(collider2);

	Color color;
	color = bCheck ? Color(1, 0, 0, 1) : Color(0, 1, 0, 1);

	collider->Render(color);
	collider2->Render(color);
}