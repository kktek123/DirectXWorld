#include "stdafx.h"
#include "TerrainBrushDemo.h"
#include "Viewer/Freedom.h"

void TerrainBrushDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 0, 0);
	Context::Get()->GetCamera()->Position(44, 51, -49);
	((Freedom *)Context::Get()->GetCamera())->Speed(80, 2);


	shader = new Shader(L"17_Terrain_Brush.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
}

void TerrainBrushDemo::Destroy()
{
	
}

void TerrainBrushDemo::Update()
{
	sky->Update();
	terrain->Update();
}

void TerrainBrushDemo::Render()
{
	sky->Render();
	terrain->Render();
}