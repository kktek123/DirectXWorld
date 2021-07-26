#include "stdafx.h"
#include "WeatherDemo.h"
#include "Environment/Rain.h"
#include "Environment/Snow.h"

void WeatherDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(24, 0, 0);
	Context::Get()->GetCamera()->Position(128, 52, -62);
	((Freedom *)Context::Get()->GetCamera())->Speed(100, 2);


	shader = new Shader(L"19_Terrain_Splatting.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->LayerMap(L"Terrain/Grass (Lawn).jpg", L"Terrain/Gray256.png");
	terrain->Pass(0);


	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	rain = new Rain(Vector3(300, 100, 500), (UINT)1e+4, L"Environment/Rain.png");
	snow = new Snow(Vector3(300, 100, 500), (UINT)1e+5, L"Environment/Snow.png");
}

void WeatherDemo::Update()
{
	sky->Update();
	terrain->Update();

	
	UINT selected = (UINT)weatherType;

	ImGui::Separator();
	ImGui::InputInt("Weather", (int *)&selected);
	selected %= 3;
	weatherType = (WeatherType)selected;


	switch (weatherType)
	{
		case WeatherType::Rain: rain->Update(); break;
		case WeatherType::Snow: snow->Update(); break;
	}
}

void WeatherDemo::Render()
{
	sky->Render();
	terrain->Render();

	
	switch (weatherType)
	{
		case WeatherType::Rain: rain->Render(); break;
		case WeatherType::Snow: snow->Render(); break;
	}
}