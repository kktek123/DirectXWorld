#include "stdafx.h"
#include "TerrainDemo.h"
#include "Environment/Terrain.h"

void TerrainDemo::Initialize()
{
	shader = new Shader(L"08_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->Pass(0);
}

void TerrainDemo::Destroy()
{
	SafeDelete(terrain);
}

void TerrainDemo::Update()
{
	terrain->Update();
}

void TerrainDemo::Render()
{
	terrain->Render();
}