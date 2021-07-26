#include "stdafx.h"
#include "TerrainLodDemo.h"
#include "Environment/Terrain.h"

void TerrainLodDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(0, 10, -30);
	((Freedom *)Context::Get()->GetCamera())->Speed(100, 2);


	shader = new Shader(L"47_TerrainLod.fx");

	//Terrain
	{
		TerrainLod::InitializeDesc desc =
		{
			shader,
			L"Terrain/Gray512.png",
			1.0f, 16, 5
		};

		terrain = new TerrainLod(desc);
		terrain->BaseMap(L"Terrain/Dirt.png");
		terrain->LayerMap(L"Terrain/Grass (Lawn).jpg", L"Terrain/Gray512.png");
		terrain->NormalMap(L"Terrain/Dirt_Normal.png");
		//terrain->Pass(0);
		terrain->Pass(1);
	}
}

void TerrainLodDemo::Destroy()
{
	SafeDelete(terrain);
}

void TerrainLodDemo::Update()
{
	terrain->Update();
}

void TerrainLodDemo::Render()
{
	terrain->Render();
}