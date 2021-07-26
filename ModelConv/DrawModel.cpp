#include "stdafx.h"
#include "DrawModel.h"
#include "Converter.h"

void DrawModel::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(0, 5, -15);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);

	shader = new Shader(L"27_Model.fx");


	Airplane();
	Tower();
	Tank();
}

void DrawModel::Update()
{
	if (airplane != NULL) airplane->Update();
	if (tower != NULL) tower->Update();
	if (tank != NULL) tank->Update();
}

void DrawModel::Render()
{
	if (airplane != NULL) airplane->Render();
	if (tower != NULL) tower->Render();
	if (tank != NULL) tank->Render();
}

void DrawModel::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMaterial(L"Weapon/Bow");
	airplane->ReadMesh(L"Weapon/Bow");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = airplane->AddTransform();

		transform->Position(Vector3(x, 0.0f, 2.5f));
		transform->RotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->Scale(0.1, 0.1, 0.1);
	}
	airplane->UpdateTransforms();

	airplane->Pass(1);
}

void DrawModel::Tower()
{
	tower = new ModelRender(shader);
	airplane->ReadMaterial(L"Weapon/LongArrow");
	airplane->ReadMesh(L"Weapon/LongArrow");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tower->AddTransform();

		transform->Position(Vector3(x, 0.0f, 7.5f));
		transform->RotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->Scale(0.1, 0.1, 0.1);
	}
	tower->UpdateTransforms();

	tower->Pass(1);
}

void DrawModel::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMaterial(L"Tank/Tank");
	tank->ReadMesh(L"Tank/Tank");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tank->AddTransform();

		transform->Position(Vector3(x, 0.0f, 5.0f));
		transform->RotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->Scale(0.1f, 0.1f, 0.1f);
	}
	tank->UpdateTransforms();

	tank->Pass(1);
}