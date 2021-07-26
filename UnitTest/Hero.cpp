#include "stdafx.h"
#include "Hero.h"
#include "BowMaster.h"
#include "Zombie.h"

#include "Objects/Projector.h"

Hero::Hero(Shader* shader, Shader* val)
	:shader(shader)
{
	//Context::Get()->GetCamera()->RotationDegree(11, 0, 0);
	//Context::Get()->GetCamera()->Position(0, 34, -94);
	//((Freedom *)Context::Get()->GetCamera())->Speed(100, 2);
	mtransform = new Transform();
	wtransform = new Transform();
	stransform = new Transform();
	ctransform = new Transform();
	Weapons();
	Kachujin();

	wtransform->Position(0, 100, 0);
	wtransform->Scale(60, 200, 40);
	Playercol = new Collider(mtransform, wtransform);
	ctransform->Position(-80.0, 160.0, -50.0);
	ctransform->Scale(30.f, 30.f, 80.f);
	//ctransform->Position(0, 0, -70);
	//ctransform->Scale(30.f, 30.f, 140.f);
	Swordcol = new Collider(stransform, ctransform);
	{
		hpImg = new Texture(L"Green.png");
		hpRender = new Render2D(val,0);
		//hpRender->Pass(val);
		hpRender->GetTransform()->Scale(300, 50, 1);
	}
	particle = new ParticleSystem(L"new");
}

Hero::~Hero()
{
	SafeDelete(kachujin);
	SafeDelete(Playercol);

	SafeDelete(Swordcol);

}

void Hero::Update()
{
	Terrain* Getterrain = NULL;
	KachujinAI(Getterrain);
	
	kachujin->Update();
	
	if (kachujin != NULL)
	{
		
		//kachujin->ComputeUpdate();

		//Collider
		{
			Playercol->GetTransform()->World(kachujin->GetBoneAnimationMatrix(0, 0));//히어로 충돌체 위치
			//Playercol->GetTransform()->Position(0, 10, 0);
			//Playercol->GetTransform()->Scale(10, 20, 10);
			Swordcol->GetTransform()->World(kachujin->GetBoneAnimationMatrix(0,35));//무기 충돌체 위치

			//Swordcol->GetTransform()->Scale(20, 20, 20);
			kachujin->UpdateTransforms();
		}

	}
	Playercol->Update();
	Swordcol->Update();	
}

void Hero::Update(Terrain * terrain)
{
	Terrain* Getterrain = terrain;
	KachujinAI(Getterrain);

	kachujin->Update();

	ImGui::Checkbox("Imotal", &bImotal);

	if (kachujin != NULL)
	{

		//kachujin->ComputeUpdate();

		//Collider
		{
			Playercol->GetTransform()->World(kachujin->GetBoneAnimationMatrix(0, 0));//히어로 충돌체 위치
			//Playercol->GetTransform()->Position(0, 10, 0);
			//Playercol->GetTransform()->Scale(10, 20, 10);
			Swordcol->GetTransform()->World(kachujin->GetBoneAnimationMatrix(0, 35));//무기 충돌체 위치
			particle->GetTransform()->World(kachujin->GetBoneAnimationMatrix(0, 35));
			Vector3 position = Vector3(0, 0, -100);
			if(bpowerup)
				particle->Add();
			particle->GetTransform()->Position(particle->GetTransform()->GetPosition() - position);

			//Swordcol->GetTransform()->Scale(20, 20, 20);
			kachujin->UpdateTransforms();
		}

	}
	Playercol->Update();
	Swordcol->Update();
	particle->Update();
}

void Hero::Render()
{
	if (kachujin != NULL) kachujin->Render();
	//Playercol->Render(Color(0, 1, 0, 1));
	//Swordcol->Render(Color(0, 1, 0, 1));
	

}

void Hero::ParticleRender()
{
	particle->Render();
}

void Hero::KachujinAI(Terrain* terrain)
{
	UIUpdate();

	GetItem();
	Move(terrain);
	Action();
}

vector<pair<function<bool()>, Render2D*>> Hero::GetUI()
{
	vector<pair<function<bool()>, Render2D*>> ui;
	ui.push_back(make_pair([&]()->bool
	{
		hpRender->SRV(hpImg->SRV());
		return true;
	}, hpRender)
	);
	return ui;
}

void Hero::Attack(BowMaster * enemy)
{
	for (int i = 0; i < enemy->GetCount(); i++)
	{
		if (enemy->CanImpact(i) == false) continue;
		
		if (a_attack.Attacking())
		{
			if (Swordcol->IsIntersect(enemy->GetHurtCollider(i)))
			{
				enemy->Damage(i, 20);
			}
		}
		
	}
}

void Hero::Attack(Zombie * enemy)
{
	for (int i = 0; i < enemy->GetCount(); i++)
	{
		if (enemy->CanImpact(i) == false) continue;

		if (a_attack.Attacking())
		{
			if (Swordcol->IsIntersect(enemy->GetHurtCollider(i)))
			{
				enemy->Damage(i, 20);
			}
		}

	}
}

void Hero::Damage(float damage)
{
	if (state == State::Death) return;
	hp -= damage;

	attackTimer.Stop();
	a_attack.Stop();
	if (hp <= 0)
	{
		ChangeState(State::Death, 1.5, 0.5f);

		hitTimer.Start([&]
		{
			//kachujin->EraseTransform(0);
			kachujin->Stop(0);
		}, kachujin->GetModel()->ClipByIndex(State::Death)->Duration() / 40 * 1000 - 1, 1);
	}
	else
	{
		ChangeState(State::Hit, 1.5, 0);

		hitTimer.Start([&]
		{
			ChangeState(State::Idle, 1.5, 0);
		}, kachujin->GetModel()->ClipByIndex(State::Hit)->Duration() / 40 * 1000, 1);
	}

}

void Hero::Move(Terrain* terrain)
{
	if ((state ==State::Idle || state == State::Walk || state == State::Run) == false)
		return;
	State moveState = State::Idle;
	Vector3 pos, rot, dir;
	transform->Position(&pos);
	transform->Rotation(&rot);
	Vector3 forward = transform->Forward();

	//Context::Get()->GetCamera()->Rotation(&rot);
	//rot.x = rot.z = 0;
	//rot.y += Math::PI;
	if (Mouse::Get()->Press(1) == false)
	{
		if (Keyboard::Get()->Press(VK_SHIFT))
		{
			moveState = State::Run;
			speed = 400;
		}
		else
		{
			moveState = State::Walk;
			speed = 200;
		}
		if (Keyboard::Get()->Press('W'))
		{
			pos -= transform->Forward() * speed * Time::Delta();
			//if (Out)
			//	mposition += transform->Forward() * speed * 5 * Time::Delta();
		}
		else if (Keyboard::Get()->Press('S'))
		{
			pos += transform->Forward() * speed * Time::Delta();
			//if (Out)
			//	mposition -= transform->Forward() * speed * 5 * Time::Delta();
		}
		else
		{
			moveState = State::Idle;
		}
		if (Keyboard::Get()->Press('D'))
			rot.y += 2 * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			rot.y -= 2 * Time::Delta();
		pos.y = terrain->GetHeight(pos);
	}
	//Run or Walk



	//플레이어 이동에 따른 회전

		//float acosValue = acos(dir.x);
		//acosValue = -dir.z > 0 ? acosValue : 2 * Math::PI - acosValue;
		//rot.y += acosValue + Math::PI * 0.5f;

		//pos -= transform->Forward() * speed * Time::Delta();

		//사실 1프레임의 움직임 어긋남이 있다 ㅋ


	if (state != State::Hit)
		ChangeState(moveState);

	transform->Rotation(rot);
	transform->Position(pos);

	kachujin->UpdateTransforms();
	Vector3 f = Context::Get()->GetCamera()->Forward();
	if (!Getdead())
		Context::Get()->GetCamera()->Position(pos - f * 100 + Vector3(0, 20, 0));
	////if (Keyboard::Get()->Press('W'))
	////	Context::Get()->GetCamera()->Position(pos - f * 40 + Vector3(0, 20, 0));
	////if (Keyboard::Get()->Press('W') && Keyboard::Get()->Press(VK_SHIFT))
	////	Context::Get()->GetCamera()->Position(pos + f * 10 + Vector3(0, 20, 0));
	//if (!Mouse::Get()->Press(1) || Getdead())
	//	Context::Get()->GetCamera()->Rotation(rot + Vector3(0, 3.14f, 0));
}

void Hero::Action()
{
	if ((state == State::Idle || state == State::Walk || state == State::Run) == false)
		return;
	if (Mouse::Get()->Down(0))
	{
		float animTime = 0;
		if (Keyboard::Get()->Press(VK_SHIFT))
		{
			animTime = kachujin->GetModel()->ClipByIndex(State::Attack3)->Duration() / 30.0f * 500;
			ChangeState(State::Attack3, 1.4, 1);

			a_attack.Start(100, animTime - 100);

			attackTimer.Start([&] { ChangeState(Idle, 1.5, 1); }, animTime, 1);
		}
		else
		{
			animTime = kachujin->GetModel()->ClipByIndex(State::Attack1)->Duration() / 30.0f * 500;
			ChangeState(State::Attack1, 1.4, 1);

			a_attack.Start(100, animTime - 100);

			attackTimer.Start([&] { ChangeState(Idle, 1.5, 1); }, animTime, 1);
		}
		
		
	}

	if (Keyboard::Get()->Down('M'))
	{
		float animTime = 0;

		animTime = kachujin->GetModel()->ClipByIndex(State::spatk)->Duration() / 30.0f * 1000;
		ChangeState(State::spatk, 1, 0);

		a_attack.Start(100, animTime - 100);

		specialAttack = true;
		attackTimer.Start([&] { ChangeState(Idle, 1, 0); specialAttack = false; }, animTime, 1);		
	}


	if (Keyboard::Get()->Down('P'))
	{
		float animTime = 0;

		animTime = kachujin->GetModel()->ClipByIndex(State::powerup)->Duration() / 30.0f * 1000;
		ChangeState(State::powerup, 1, 0);

		particle->Update();
		a_attack.Start(100, animTime - 100);

		bpowerup = true;
		attackTimer.Start([&] { ChangeState(Idle, 1, 0); //specialAttack = false;
		}, animTime, 1);
	}

}

void Hero::UIUpdate()
{
	Vector3 scale = hpRender->GetTransform()->GetScale();
	scale.x = hp * 3;

	hpRender->GetTransform()->Scale(scale);
	hpRender->GetTransform()->Position(scale.x*0.5f, D3D::Height() - scale.y*0.5f, 0);
}

void Hero::GetItem()
{
}

void Hero::ChangeState(State state, float speed, float takeTime)
{
	this->state = state;
	kachujin->PlayClip(0, state, speed, takeTime);

}

Vector3 Hero::GetDirection(State * moveState)
{
	

	//Move
	if (Keyboard::Get()->Press('W'))
		direction.z = 1;
	else if (Keyboard::Get()->Press('S'))
		direction.z = -1;

	if (Keyboard::Get()->Press('D'))
		direction.x = 1;
	else if (Keyboard::Get()->Press('A'))
		direction.x = -1;

	if (direction == Vector3(0, 0, 0))
		*moveState = State::Idle;

	D3DXVec3Normalize(&direction, &direction);

	return direction;
}




void Hero::Pass(UINT val)
{
	kachujin->Pass(val);
}

void Hero::Weapons()
{
	vector<wstring> names;
	names.push_back(L"Cutter");
	names.push_back(L"Cutter2");
	names.push_back(L"Dagger_epic");
	names.push_back(L"Dagger_small");
	names.push_back(L"Katana");
	names.push_back(L"Rapier");
	names.push_back(L"Sword");
	names.push_back(L"Sword2");
	names.push_back(L"Sword_Epic");
	names.push_back(L"LongArrow");
	names.push_back(L"LongBow");

	for (UINT i = 0; i < names.size(); i++)
	{
		weapons[i] = new Model();
		weapons[i]->ReadMaterial(L"Weapon/" + names[i]);
		weapons[i]->ReadMesh(L"Weapon/" + names[i]);
		//if (i == 8)
		//{
		//	Matrix mats;
		//	mbone = weapons[i]->BoneByIndex(1);
		//	D3DXMatrixScaling(&mats, 0.f, 0.f, 0.f);
		//	mbone->Transform(mats);

		//}
	}


}

void Hero::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMaterial(L"Player/Mesh");
	kachujin->ReadMesh(L"Player/Mesh");
	kachujin->ReadClip(L"Player/Idle");
	kachujin->ReadClip(L"Player/Walking");
	kachujin->ReadClip(L"Player/Running");
	kachujin->ReadClip(L"Player/Hit");
	kachujin->ReadClip(L"Player/Death");
	kachujin->ReadClip(L"Player/Attack1");
	kachujin->ReadClip(L"Player/Attack2");
	kachujin->ReadClip(L"Player/Attack3");
	kachujin->ReadClip(L"Player/Powerup");
	kachujin->ReadClip(L"Player/SpAttack");

	transform = NULL;

	transform = kachujin->AddTransform();
	

	Transform attachTransform;

	attachTransform.Position(0, 10, -10);
	attachTransform.Scale(1.f, 1.f, 1.f);
	//Sword
	//attachTransform.Position(-10, -0, -10);
	//attachTransform.Scale(1.0f, 1.0f, 1.0f);
	attachTransform.RotationDegree(0, 0, 90);

	//LongBow



	kachujin->GetModel()->Attach(shader, weapons[6], 35, &attachTransform);
	

	
	
	//transform->Position(80, 0, 80);
	//transform->Scale(0.02f, 0.02f, 0.02f);
	//transform->RotationDegree(0, 90, 0);



	//kachujin->CreateTexture();
	//kachujin->UpdateTransforms();

	

}
