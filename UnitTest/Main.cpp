#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "RaycastDemo.h"
#include "FogDemo.h"
#include "TheWorldLOD.h"
#include "RefractionDemo.h"
#include "ColorToneDemo.h"
#include "DSpotLightDemo.h"
#include "DPointLightDemo.h"
#include "GBufferDemo.h"
#include "ParticleDemo.h"
#include "WaterDemo.h"
#include "TerrainLodDemo.h"
#include "CullingDemo.h"


void Main::Initialize()
{
	Push(new TheWorldLOD());
	//Push(new TerrainLodDemo());
	//Push(new FogDemo());
	//Push(new WaterDemo());
	//Push(new RaycastDemo());
	//Push(new CullingDemo());
	//Push(new RefractionDemo());
	//Push(new PostEffectsDemo());
	//Push(new BloomDemo());
	//Push(new BlurDemo());
	//Push(new ColorToneDemo());
	//Push(new DSpotLightDemo());
	//Push(new DPointLightDemo());
	//Push(new GBufferDemo());
	//Push(new ParticleDemo());
}

void Main::Ready()
{

}

void Main::Destroy()
{
	for (IExecute* exe : executes)
	{
		exe->Destroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	D3D::SetDesc(desc);

	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SafeDelete(main);

	return wParam;
}