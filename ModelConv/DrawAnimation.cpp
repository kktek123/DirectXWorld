#include "stdafx.h"
#include "DrawAnimation.h"
#include "Converter.h"

void DrawAnimation::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(26, 0, 0);
	Context::Get()->GetCamera()->Position(0, 17, -36);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);

	shader = new Shader(L"27_Animation.fx");

	Kachujin();
}

void DrawAnimation::Update()
{
	static UINT clip = 0;

	if (kachujin != NULL)
	{
		if (Keyboard::Get()->Down(VK_SPACE))
		{
			++clip;
			clip %= 4;
			kachujin->PlayClip(20, clip, 1.0f, 1.0f);
		}


		kachujin->Update();
	}
}

void DrawAnimation::Render()
{
	if (kachujin != NULL)
		kachujin->Render();
}

void DrawAnimation::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMaterial(L"Archer/Mesh");
	kachujin->ReadMesh(L"Archer/Mesh");
	kachujin->ReadClip(L"Archer/Idle");
	kachujin->ReadClip(L"Archer/Running");
	kachujin->ReadClip(L"Archer/Hit");
	kachujin->ReadClip(L"Archer/Death");

	Model* weapon = new Model();
	weapon->ReadMaterial(L"Weapon/Sword");
	weapon->ReadMesh(L"Weapon/Sword");

	Transform transform;
	transform.Position(10, -3.5f, -10);
	transform.Scale(1.28f, 1.28f, 1.28f);

	kachujin->GetModel()->Attach(shader, weapon, 35, &transform);
	kachujin->Pass(2);


	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = kachujin->AddTransform();
		transform->Position(x, 0, -5);
		transform->Scale(0.01f, 0.01f, 0.01f);
	}
	kachujin->UpdateTransforms();
}
