#include "stdafx.h"
#include "CubeSkyDemo.h"
#include "Viewer/Freedom.h"

void CubeSkyDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(24, 0, 0);
	Context::Get()->GetCamera()->Position(0, 29, -81);
	((Freedom *)Context::Get()->GetCamera())->Speed(30, 2);


	//Check(D3DX11CreateShaderResourceViewFromFile
	//(
	//	D3D::GetDevice(), L"../../_Textures/Environment/DesertCube1024.dds", NULL, NULL, &srv, NULL
	//));

	shader = new Shader(L"15_Mesh.fx");
//	shader->AsSRV("CubeMap")->SetResource(srv);

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	cube = new MeshRender(shader, new MeshCube());
	cube->AddTransform();
	cube->GetTransform(0)->Position(0, 5, 0);
	cube->GetTransform(0)->Scale(20, 10, 20);
	cube->UpdateTransforms();

	grid = new MeshRender(shader, new MeshGrid(6, 6));
	grid->AddTransform();
	grid->GetTransform(0)->Scale(12, 1, 12);
	grid->UpdateTransforms();


	cylinder = new MeshRender(shader, new MeshCylinder(0.5f, 3.0f, 20, 20));
	sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
	for (UINT i = 0; i < 5; i++)
	{
		cylinder->AddTransform();
		cylinder->GetTransform(i)->Position(-30, 6, -15.0f + (float)i * 15.0f);
		cylinder->GetTransform(i)->Scale(5, 5, 5);

		cylinder->AddTransform();
		cylinder->GetTransform(i)->Position(30, 6, -15.0f + (float)i * 15.0f);
		cylinder->GetTransform(i)->Scale(5, 5, 5);


		sphere->AddTransform();
		sphere->GetTransform(i)->Position(-30, 15.5f, -15.0f + (float)i * 15.0f);
		sphere->GetTransform(i)->Scale(5, 5, 5);

		sphere->AddTransform();
		sphere->GetTransform(i)->Position(30, 15.5f, -15.0f + (float)i * 15.0f);
		sphere->GetTransform(i)->Scale(5, 5, 5);
	}
	cylinder->UpdateTransforms();
	sphere->UpdateTransforms();
}

void CubeSkyDemo::Destroy()
{
	
}

void CubeSkyDemo::Update()
{
	sky->Update();

	cylinder->Update();
	sphere->Update();
	cube->Update();
	grid->Update();
}

void CubeSkyDemo::Render()
{
	sky->Render();

	grid->Render();
	cube->Render();
	cylinder->Render();
	sphere->Render();
}