#include "stdafx.h"
#include "ParticleDemo.h"

void ParticleDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0, 0);
	Context::Get()->GetCamera()->Position(0, 32, -67);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);


	shader = new Shader(L"43_Water.fxo");
	shadow = new Shadow(shader, Vector3(0, 0, 0), 65);
	
	sky = new CubeSky(L"Environment/GrassCube1024.dds", shader);
	
	Mesh();
	
	particle = new ParticleSystem(L"Smoke");
	//particle = new ParticleSystem(L"Star");
	//particle = new ParticleSystem(L"Explosion");
}

void ParticleDemo::Destroy()
{

}

void ParticleDemo::Update()
{
	Vector3 position;
	cube->GetTransform(0)->Position(&position);

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

	cube->GetTransform(0)->Position(position);
	cube->UpdateTransforms();


	sky->Update();
	
	cube->Update();
	grid->Update();

	particle->Add(position);
	particle->Update();
}

void ParticleDemo::PreRender()
{
	//Depth
	{
		shadow->PreRender();

		floor->Render();
		
		grid->Pass(1);
		grid->Render();

		brick->Render();
		cube->Pass(1);
		cube->Render();
	}
}

void ParticleDemo::Render()
{
	sky->Pass(0);
	sky->Render();

	floor->Render();
	grid->Pass(4);
	grid->Render();

	brick->Render();
	cube->Pass(4);
	cube->Render();

	particle->Render();
}

void ParticleDemo::PostRender()
{
	
}

void ParticleDemo::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->Specular(1, 1, 1, 15);
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->Specular(1, 0.3f, 0.3f, 2);
		brick->SpecularMap("Bricks_Specular.png");
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
		brick->NormalMap("Bricks_Normal.png");
	}

	//Create Mesh
	{
		Transform* transform = NULL;

		cube = new MeshRender(shader, new MeshCube());
		transform = cube->AddTransform();
		transform->Position(0, 10, 0);
		transform->Scale(3, 3, 3);

		grid = new MeshRender(shader, new MeshGrid(5, 5));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		//transform->Position(0, 5, 0);
		transform->Scale(12, 1, 12);
	}

	cube->UpdateTransforms();
	grid->UpdateTransforms();
}