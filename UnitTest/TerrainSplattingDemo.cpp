#include "stdafx.h"
#include "TerrainSplattingDemo.h"
#include "Viewer/Freedom.h"

void TerrainSplattingDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 0, 0);
	Context::Get()->GetCamera()->Position(44, 51, -49);
	((Freedom *)Context::Get()->GetCamera())->Speed(80, 2);


	shader = new Shader(L"19_Terrain_Splatting.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->LayerMap(L"Terrain/Grass (Hill).jpg", L"Terrain/Gray256.png");
}

void TerrainSplattingDemo::Destroy()
{
	
}

void TerrainSplattingDemo::Update()
{
	sky->Update();
	terrain->Update();
}

void TerrainSplattingDemo::Render()
{
	sky->Render();
	terrain->Render();
}