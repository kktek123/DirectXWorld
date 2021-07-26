#include "stdafx.h"
#include "BillboardDemo.h"

void BillboardDemo::Initialize()
{
	shader = new Shader(L"19_Terrain_Splatting.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->LayerMap(L"Terrain/Grass (Lawn).jpg", L"Terrain/Gray256.png");
	terrain->Pass(0);


	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	billboardShader = new Shader(L"26_Billboard.fx");
	
	billboard = new Billboard(billboardShader);
	billboard->AddTexture(L"White.png");
	billboard->AddTexture(L"Red.png");
	billboard->AddTexture(L"Blue.png");
	billboard->AddTexture(L"Green.png");
	billboard->AddTexture(L"Box.png");
	billboard->AddTexture(L"Bricks.png");

	for (UINT i = 0; i < 100; i++)
	{
		Vector2 scale = Math::RandomVec2(3.0f, 10.0f);
		Vector3 position = Math::RandomVec3(0.0f, 255.0f);
		position.y = terrain->GetHeight(position) + scale.y * 0.5f;
		
		billboard->Add(position, scale);
	}
}

void BillboardDemo::Update()
{
	sky->Update();
	terrain->Update();

	billboard->Update();
}

void BillboardDemo::Render()
{
	sky->Render();
	terrain->Render();

	billboard->Render();
}