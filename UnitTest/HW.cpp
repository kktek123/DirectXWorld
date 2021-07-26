#include "stdafx.h"
#include "HW.h"

#include "Viewer/Freedom.h"
#include "Renders/Context.h"
#include "Model/ModelClip.h"

#include "Environment/CubeSky.h"
#include "Objects/Projector.h"

#include "Zombie.h"
#include "BowMaster.h"

#include "Hero.h"


void HW::Initialize()
{
	//Context::Get()->GetCamera()->RotationDegree(11, 0, 0);
	//Context::Get()->GetCamera()->Position(0, 34, -94);
	//((Freedom *)Context::Get()->GetCamera())->Speed(100, 2);
	shader = new Shader(L"038_Projector.fxo");
	shader2D = new Shader(L"030_Render2D.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");
	projector = new Projector(shader, L"Environment/MagicCircle.png",100,100);

	//struct Node
	//{
	//	int Data;
	//	int* Point;
	//};

	//Node* data = new Node();
	//data->Point = new int();

	{
		player = new Hero(shader, shader2D);//Create player
		player->GetTransform()->Position(0, 0, 0);
		player->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
		//player->Update();

		vector<UI>ui = player->GetUI();//Create UI
		uis.assign(ui.begin(), ui.end());//���� hp��
	}
	{

		//bowMaster = new BowMaster(shader);//Create bowMaster
		//Transform* transform = NULL;
		//countbowm = 10;
		//for (int i = 0; i < 10; i++)
		//{
		//	Vector3 random = Math::RandomVec3(-300, 300);
		//	random.y = 0;

		//	transform = bowMaster->AddTransform();
		//	transform->Position(random);
		//	transform->Scale(0.1f, 0.1f, 0.1f);

		//}
		//bowMaster->InitTransforms();
		//bowMaster->SetTraget(player->GetTransform());	//Ÿ������
		//bowMaster->SetFindRange(100);					//Ž������
		//bowMaster->SetAttackRange(50);					//���ݹ���
		//bowMaster->SetSpeed(100);						//�̵��ӵ�
		//bowMaster->CreateRandomPath(10, 4);				//��ȸ������ ��ȸ ����Ʈ

		//vector<UI> ui = bowMaster->GetUI();				//���� hp
		//for (int i = 0; i < ui.size(); i++)
		//	uis.push_back(ui[i]);
	}
	{

		zombie = new Zombie(shader, shader2D);//Create bowMaster
		Transform* transform = NULL;
		countbowm = 10;
		for (int i = 0; i < 7; i++)
		{
			Vector3 random = Math::RandomVec3(-300, 300);
			random.y = 0;

			//transform = zombie->AddTransform();
			transform->Position(random);
			transform->Scale(0.1f, 0.1f, 0.1f);

		}
		zombie->InitTransforms();
		zombie->SetTraget(player->GetTransform());	//Ÿ������
		zombie->SetFindRange(100);					//Ž������
		zombie->SetAttackRange(50);					//���ݹ���
		zombie->SetSpeed(100);						//�̵��ӵ�
		zombie->CreateRandomPath(10, 4);				//��ȸ������ ��ȸ ����Ʈ

		vector<UI> ui = zombie->GetUI();				//���� hp
		for (int i = 0; i < ui.size(); i++)
			uis.push_back(ui[i]);
	}
	Mesh();
}

void HW::Destroy()
{
	SafeDelete(shader);

	SafeDelete(player);

	//SafeDelete(bowMaster);

	//SafeDelete(zombie);
}

void HW::Update()
{

	projector->Update();
	sky->Update();
	//Player Update
	{
		//Vector3 position;  player->GetTransform()->Position(&position);
		//float distance1, distance2;

		//Transform* tr = bowMaster->GetNearInstance(position, &distance1);//�÷��̾�Ÿ��


		//player->Update();
	}
	//bowMaster Update
	{
		//bowMaster->Update();
	}
	{
		//zombie->Update();
	}

	//Collider Check
	{
		//Player Attack
		//if(bowMaster->IsDead(true))
		//player->Attack(bowMaster);
		player->Attack(zombie);


		//bowMaster Attack
		//bowMaster->Attack(player);

		zombie->Attack(player);
	}
	Vector3 porjpos = player->GetTransform()->GetPosition();
	if (player->Getspatk())//��������
	{
		projector->GetCamera()->Position(porjpos.x,30, porjpos.z);
		for (int i = 0; i < bowMaster->GetCount(); i++)
		{
			Vector3 bowMasterpos = bowMaster->GetHurtCollider(i)->GetTransform()->GetPosition();//���� �浹ü��ġ
			float dx = bowMasterpos.x - porjpos.x;
			float dz = bowMasterpos.z - porjpos.z;
				float dist = sqrt(dx * dx + dz * dz);
			if (dist <= 100)//�������� ������ ����� ����
			{
				bowMaster->Damage(i, 20);
			}

		}

		for (int i = 0; i < zombie->GetCount(); i++)
		{
			Vector3 zombiepos = zombie->GetHurtCollider(i)->GetTransform()->GetPosition();//���� �浹ü��ġ
			float dx = zombiepos.x - porjpos.x;
			float dz = zombiepos.z - porjpos.z;
			float dist = sqrt(dx * dx + dz * dz);
			if (dist <= 100)//�������� ������ ����� ����
			{
				zombie->Damage(i, 20);
			}

		}
		
	}
	else
		projector->GetCamera()->Position(0,0,0);
	grid->Update();
}

void HW::Render()
{
	
	projector->Render();
	sky->Render();

	//bowMaster->Render();
	zombie->Render();
	player->Render();

	floor->Render();
	grid->Render();

	for (UI& ui : uis)
		if (ui.first()) ui.second->Render();

	
}

void HW::Mesh()
{
	Transform* transform = NULL;
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->SpecularMap("Floor_Specular.png");
	floor->NormalMap("Floor_Normal.png");
	floor->Specular(1, 1, 1, 20);

	grid = new MeshRender(shader, new MeshGrid(20, 20));
	transform = grid->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(100, 1, 100);

	grid->Pass(0);
	grid->UpdateTransforms();
}
