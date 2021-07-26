#include "stdafx.h"
#include "TerrainTextureDemo.h"
#include "Viewer/Freedom.h"
#include "Environment/Terrain.h"

void TerrainTextureDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 0, 0);
	Context::Get()->GetCamera()->Position(44, 51, -49);
	((Freedom *)Context::Get()->GetCamera())->Speed(80, 2);


	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->Pass(0);
}

void TerrainTextureDemo::Destroy()
{
	
}

void TerrainTextureDemo::Update()
{
	terrain->Update();
}

void TerrainTextureDemo::Render()
{
	terrain->Render();
}