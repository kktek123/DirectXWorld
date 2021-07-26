#include "stdafx.h"
#include "TheWorld.h"
#include "Viewer/Freedom.h"

#include "Environment/Terrain.h"
#include "Environment/TerrainRender.h"
#include "Environment/TerrainLOD.h"
#include "Environment/Sky.h"
#include "Environment/CubeSky.h"
#include "Environment/LakeWater.h"

void TheWorld::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(13, 0, 0);
	Context::Get()->GetCamera()->Position(133, 29, -45);
	((Freedom *)Context::Get()->GetCamera())->Speed(100, 2);

	shader = new Shader(L"049_TheWorld.fx");
	shadow = new Shadow(shader, Vector3(0, 0, 0), 1000);
	sky = new Sky(shader);
	sky->ScatteringPass(3);
	sky->RealTime(false, Math::PI - 1e-6f);

	lakeWater = new LakeWater(shader, 5, 100,256,256);
	

	//Terrain
	{
		Transform* transform = NULL;
		terrain = new Terrain(shader, L"Terrain/heightmap01.bmp");
		terrain->BaseMap(L"Terrain/dirt01.dds");
		terrain->NormalMap(L"Terrain/dirt01.dds");
		terrain->DetailMap(L"Terrain/detail001.dds");
		terrain->SpecularMap(L"Terrain/dirt01.dds");
		//terrain->LayerMap(L"Terrain/Snow.jpg", L"Terrain/Splatting.png");
		Rterrain = new TerrainRender(shader, terrain);

		transform = Rterrain->AddTransform();
		transform->Position(0, 0, 0);
		transform->Scale(1, 1, 1);
	}
	Rterrain->UpdateTransforms();

	//
	////Terrain
	Mesh();
	Airplane();
	Kachujin();
	//terrain->Pass();
	//
}

void TheWorld::Destroy()
{
	
}

void TheWorld::Update()
{
	UINT& type = Context::Get()->FogType();
	ImGui::InputInt("FogType", (int*)&type);
	type %= 4;
	ImGui::ColorEdit3("FogColor", Context::Get()->FogColor());
	ImGui::SliderFloat("FogDensity", &Context::Get()->FogDensity(), 0, 1, "% 2f");
	ImGui::SliderFloat("FogDistancX", &Context::Get()->FogDistance().x, 0, 10.0f);
	ImGui::SliderFloat("FogDistancY", &Context::Get()->FogDistance().y, 0, 300.0f);

	Vector3 position;
	airplane->GetTransform(0)->Position(&position);

	if (Keyboard::Get()->Press('L'))
		position.x += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('J'))
		position.x -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('I'))
		position.z += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('O'))
		position.y += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('U'))
		position.y -= 20 * Time::Delta();

	airplane->GetTransform(0)->Position(position);
	airplane->UpdateTransforms();
	sky->Update();
	grid->Update();
	Rterrain->Update();
	if (airplane != NULL) airplane->Update();
	if (kachujin != NULL) kachujin->Update();


	lakeWater->Update();
}

void TheWorld::PreRender()
{
	sky->PreRender();
	//Shadow Depth
	{
		shadow->Set();
		Rterrain->Pass(22);
		Rterrain->Render();
		Pass(0, 1, 2);
		floor->Render();
		grid->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujin != NULL) kachujin->Render();
		
	}
	//Reflection PreRender
	{
		lakeWater->SetReflection();
		sky->Pass(11, 12, 13);
		sky->Render();
		Rterrain->Pass(20);
		Rterrain->Render();
		Pass(8, 9, 10);
		floor->Render();
		grid->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujin != NULL) kachujin->Render();

	}
	//Refraction PreRender
	{
		lakeWater->SetRefraction();

		sky->Pass(4, 5, 6);
		sky->Render();
		Rterrain->Pass(21);
		Rterrain->Render();
		Pass(16, 17, 18);
		floor->Render();
		grid->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujin != NULL) kachujin->Render();
		
	}

}

void TheWorld::Render()
{
	lakeWater->ResetClipPlane();

	sky->Pass(4, 5, 6);
	sky->Render();
	Rterrain->Pass(21);
	Rterrain->Render();
	Pass(16, 17, 18);
	floor->Render();
	grid->Render();
	if (airplane != NULL)
		airplane->Render();
	if (kachujin != NULL)
		kachujin->Render();

	lakeWater->Pass(19);
	lakeWater->Render();

}

void TheWorld::Mesh()
{
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->SpecularMap("Floor_Specular.png");
	floor->NormalMap("Floor_Normal.png");
	floor->Specular(1, 1, 1, 20);

	//Create Mesh
	{
		Transform* transform = NULL;


		grid = new MeshRender(shader, new MeshGrid(5, 5));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		transform->Scale(20, 1, 20);
	}
	grid->UpdateTransforms();
	meshes.push_back(grid);
}

void TheWorld::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMaterial(L"B787/Airplane");
	airplane->ReadMesh(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(200.0f, 9.91f, 200.0f);
	transform->Scale(0.02, 0.02, 0.02);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void TheWorld::Kachujin()
{
	weapon = new Model();
	weapon->ReadMaterial(L"Weapon/Sword");
	weapon->ReadMesh(L"Weapon/Sword");

	kachujin = new ModelAnimator(shader);
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Idle");
	//kachujin->ReadClip(L"Kachujin/Running");
	//kachujin->ReadClip(L"Kachujin/Jump");
	//kachujin->ReadClip(L"Kachujin/Hip_Hop_Dancing");

	Transform attachTransform;

	attachTransform.Position(-10, 0, -10);
	attachTransform.Scale(1.0f, 1.0f, 1.0f);

	kachujin->GetModel()->AttachModel(shader, weapon, 35, &attachTransform);


	Transform* transform = NULL;

	transform = kachujin->AddTransform();
	transform->Position(200, 0, 210);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayClip(0, 0, 1.0f);
	kachujin->UpdateTransforms();
	animators.push_back(kachujin);
}

void TheWorld::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}
