#include "stdafx.h"
#include "TerrainLineDemo.h"
#include "Viewer/Freedom.h"

void TerrainLineDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 0, 0);
	Context::Get()->GetCamera()->Position(44, 51, -49);
	((Freedom *)Context::Get()->GetCamera())->Speed(80, 2);


	shader = new Shader(L"18_Terrain_Line.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
}

void TerrainLineDemo::Destroy()
{
	
}

void TerrainLineDemo::Update()
{
	sky->Update();
	terrain->Update();
}

void TerrainLineDemo::Render()
{
	sky->Render();
	terrain->Render();
}