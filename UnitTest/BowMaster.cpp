#include "stdafx.h"
#include "BowMaster.h"
#include "Hero.h"
#include "Viewer/Freedom.h"
#include "Environment/CubeSky.h"
#include "Environment/Terrain.h"
#include "Model/ModelMesh.h"
#include "Model/ModelAnimator.h"
#include "Model/ModelClip.h"
#include "Model/Model.h"


BowMaster::BowMaster(Shader* shader, Shader* tr)
	:shader(shader), shader2D(tr)
{
	//ctransform = new Transform();
	Weapons();
	BowMasterEnemy();
	hpImg = new Texture(L"Red.png");

}
BowMaster::~BowMaster()
{
	SafeDelete(enemy);

	for (int i = 0; i < hitColliders.size(); i++)
		SafeDelete(hitColliders[i]);
		
	for (int i = 0; i < attackColliders.size(); i++)
		SafeDelete(attackColliders[i]);
	SafeDelete(arrow);
}

void BowMaster::Update(Terrain* terrain)
{
	Move(terrain);
	for (UINT i = 0; i < hitColliders.size(); i++)
	{
		hitColliders[i]->Update();
	}
	for (UINT i = 0; i < attackColliders.size(); i++)
	{
		attackColliders[i]->Update();
	}
	for (UINT i = 0; i < hitColliders.size(); i++)
	{
		//hitColliders[i]->GetTransform()->World(enemy->GetAttach(i, 0));
		//hitColliders[i]->GetTransform()->Scale(2.f, 4.0f, 2.0f);

	}
	//enemy->UpdateTransforms();
	//arrow->UpdateTransforms();
	if (enemy != NULL)
	{
		enemy->Update();
		//enemy->ComputeUpdate();
		enemy->UpdateTransforms();
		
	}
	ArrowUpdate();
	arrow->UpdateTransforms();
	for (int i = 0; i < hpRenders.size(); i++)
	{
		Render2D* hpRender = hpRenders[i];
		Vector3 position;
		Context::Get()->GetViewport()->Project(&position, hpPosition, enemy->GetTransform(i)->World(), Context::Get()->View(), Context::Get()->Projection());
		position.y = D3D::Height() - position.y;

		if (position > 0 && position.z<1)
		{
			position.z = 0;
		}
		Vector3 scale = hpRender->GetTransform()->GetScale();
		scale.x = hp[i] * (200.0f / 40.0f);//길이 / 최대 체력 = 체력 1당 길이
		scale.y = hp[i] * 0.1f;
		position.x -= 100;

		hpRender->GetTransform()->Scale(scale);
		hpRender->GetTransform()->Position(position);
		hpRender->Update();
	}
}

void BowMaster::Render()
{

	if (enemy != NULL) enemy->Render();
	if (arrow != NULL)
	{
		arrow->Render();
	}
	//for (UINT i = 0; i < hitColliders.size(); i++)
	//{
	//	if (!dead[i])
	//	hitColliders[i]->Render(Color(0,1,0,1));
	//}
	//for (UINT i = 0; i < attackColliders.size(); i++)
	//{
	//	attackColliders[i]->Render(Color(0, 1, 0, 1));
	//}
}


Transform * BowMaster::AddTransform(float health = 100, float attack = 40, float defend = 0)
{
	Transform* tr = enemy->AddTransform();
	Transform* arrawTr = arrow->AddTransform();
	//Hit Collider
	{
		Transform* cTr = new Transform();
		cTr->SetShader(shader);
		cTr->Position(0, 100, 0);
		cTr->Scale(60, 200, 40);
		Collider* collider = new Collider(tr, cTr);

		hitColliders.push_back(collider);
		hitTimer.push_back(Timer());
	}
	//Attack Collider 
	{
		//Collider* collider = NULL;
		Transform* cTr = new Transform();
		

		cTr->SetShader(shader);
		cTr->Position(0, -10, 1);
		cTr->Scale(0.2, 0.2, 0.2);
		Collider* collider = new Collider(arrawTr, cTr);
		DisableArrow(arrawTr);

		attackColliders.push_back(collider);
		atkTimer.push_back(Timer());
		drawTimer.push_back(Timer());
		a_attacks.push_back(AttackTimer());
	}
	//Hp setting
	{
		Render2D* hpRender = new Render2D(shader2D,0);
		hpRender->GetTransform()->Scale(300, 20, 1);
		hp.push_back(health);
		hpRenders.push_back(hpRender);

		dead.push_back(false);
	}
	//atk,def setting
	{
		atk.push_back(attack);
		def.push_back(defend);
	}

	return tr;
}

void BowMaster::InitTransforms()
{
	enemy->UpdateTransforms();
	arrow->UpdateTransforms();
}

void BowMaster::Destroy()
{
	//enemy->EraseTransform();

}



void BowMaster::Weapons()
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

	}
	arrow = new ModelRender(shader);
	arrow->ReadMaterial(L"Weapon/LongArrow");
	arrow->ReadMesh(L"Weapon/LongArrow");
}


void BowMaster::BowMasterEnemy()
{
	enemy = new ModelAnimator(shader);
	enemy->ReadMaterial(L"Archer/Mesh");
	enemy->ReadMesh(L"Archer/Mesh");
	enemy->ReadClip(L"Archer/Idle");
	enemy->ReadClip(L"Archer/Running");
	enemy->ReadClip(L"Archer/Draw");
	enemy->ReadClip(L"Archer/Shoot");
	enemy->ReadClip(L"Archer/Hit");
	enemy->ReadClip(L"Archer/Death");


	//Enemys etransform;

	Transform attachTransform;

	//Sword
	//attachTransform.Position(10, -3.5f, -10);
	//attachTransform.Scale(1.28f, 1.28f, 1.28f);

	//LongBow
	attachTransform.Position(0, 2, 0);
	attachTransform.Scale(0.3, 0.3, 0.3);
	attachTransform.RotationDegree(180, 90, 90);


	enemy->GetModel()->Attach(shader, weapons[10], 11, &attachTransform);
	//enemy->Pass(2);


	//
	//enemy->UpdateTransforms();

}
void BowMaster::Attack(Hero * player)
{
	//if(player)
	if (player->CanHurt() == false) return;

	for (int i = 0; i < attackColliders.size(); i++)
	{
		if (a_attacks[i].Attacking() == false) continue;

		//맞음
		if (attackColliders[i]->IsIntersect(player->GetHurt()) && !player->bImotal)
		{
			atkTimer[i].Stop();
			drawTimer[i].Stop();
			//if (weaponType == WeaponType::Bow)
				DisableArrow(arrow->GetTransform(i));

			player->Damage(atk[i]);

			return;
		}
	}
}
void BowMaster::Damage(UINT i, float Damage)
{
	if (hitTimer[i].Complete() == false || dead[i]) return;
	hp[i] -= (Damage - def[i] > 0) ? (Damage - def[i]) : 0;
	if (hp[i] <= 0 && i >= 0)
	{
		dead[i] = true;
		enemy->PlayClip(i, State::Die, 1.5, 0);

		hitTimer[i].Start([&, i]
		{
			enemy->Stop(i);
			enemy->EraseTransform(i);
			hitColliders.erase(hitColliders.begin() + i);
			//arrow->EraseTransform(i);
			attackColliders.erase(attackColliders.begin() + i);
			hp.erase(hp.begin() + i);
			atk.erase(atk.begin() + i);
			def.erase(def.begin() + i);
			//atkTimer.erase(atkTimer.begin() + i);
			//drawTimer.erase(drawTimer.begin() + i);
			//hitTimer.erase(hitTimer.begin() + i);
			//a_attacks.erase(a_attacks.begin() + i);
			hpRenders.erase(hpRenders.begin() + i);
			dead.erase(dead.begin() + i);


		}, enemy->GetModel()->ClipByIndex(State::Die)->Duration() / 30 * 1000 - 1, 1);
	}
	else
	{
		enemy->PlayClip(i, State::Reaction, 1.5, 0);

		hitTimer[i].Start([&, i]
		{
			enemy->PlayClip(i, State::Idle, 1, 0);
		}, enemy->GetModel()->ClipByIndex(State::Reaction)->Duration() / 30 * 1000, 1);

	}
}

void BowMaster::CreateRandomPath(float range, UINT count)
{
	pathCount = count;

	for (int i = 0; i < enemy->GetTransformCount(); ++i)
	{
		Vector3 pos; 
		//enemy->GetTransform(i)->Position(&pos);

		Vector3* positions = new Vector3[count];
		for (int j = 0; j < count; j++)
		{
			if (j % 4 == 0)
			{
				positions[j].x = pos.x + range;
				positions[j].y = 0;
				positions[j].z = pos.z + range;
			}
			if (j % 4 == 1)
			{
				positions[j].x = pos.x + range;
				positions[j].y = 0;
				positions[j].z = pos.z - range;
			}
			if (j % 4 == 2)
			{
				positions[j].x = pos.x - range;
				positions[j].y = 0;
				positions[j].z = pos.z + range;
			}
			if (j % 4 == 3)
			{
				positions[j].x = pos.x - range;
				positions[j].y = 0;
				positions[j].z = pos.z - range;
			}
			//positions[j].x = Math::Random(pos.x - range, pos.x + range);
			//positions[j].y = enemy->GetTransform(i)->GetPosition().y;
			//positions[j].z = Math::Random(pos.z - range, pos.z + range);
		}

		//enemy->GetTransform(i)->Position(positions[0]);

		paths.push_back(positions);
		nextPathNums.push_back(1);
	}
}

void BowMaster::ChangeDirectionToPath(Vector3 & direction, UINT i, Vector3 & position)
{
	direction = paths[i][nextPathNums[i]] - position;

	if (direction.x <= 3.3f && direction.z <= 3.3f)
	{
		ChangeState(i, State::Idle, 1, 1);
		//nextPathNums[i]++;
		//if (nextPathNums[i] >= pathCount)
		//	nextPathNums[i] = 0;

		//direction = paths[i][nextPathNums[i]] - position;
	}
}


Transform * BowMaster::GetNearInstance(Vector3 position, float * outDistance)//플레이어 타겟
{
	//Transform* result = NULL;
	//*outDistance = FLT_MAX;

	//for (int i = 0; i < enemy->GetTransformCount(); i++)
	//{
	//	if (dead[i]) continue;

	//	Vector3 trPos;  enemy->GetTransform(i)->Position(&trPos);
	//	trPos = trPos - position;
	//	float dis = D3DXVec3Length(&trPos);

	//	if (dis < *outDistance)
	//	{
	//		*outDistance = dis;
	//		result = enemy->GetTransform(i);
	//	}
	//}

	//return result;
	return NULL;
}

void BowMaster::Move(Terrain* terrain)
{
	Vector3 targetPosition;
	if (target != NULL)
		target->Position(&targetPosition);
	for (int i = 0; i < enemy->GetTransformCount(); ++i)
	{
		//if (target != NULL) continue;
		if (dead[i] || atkTimer[i].Complete() == false || drawTimer[i].Complete() == false || hitTimer[i].Complete() == false) continue;

		transform = enemy->GetTransform(i);
		Vector3 position;
		transform->Position(&position);
		Vector3 direction = targetPosition - position;
		float distance = D3DXVec3Length(&direction);
		position.y = terrain->GetHeight(position);
		
		

		//float yRot = -direction.z > 0 ? acosf(direction.x) : Math::ToRadian(360) - acosf(direction.x);
		//
		//transform->Rotation(0, yRot + Math::ToRadian(90) + Math::ToRadian(180), 0);
		//Attack
		if (distance < attackRange)
		{
			Attack__(i, transform);

			D3DXVec3Normalize(&direction, &direction);
			float yRot = -direction.z > 0 ? acosf(direction.x) : Math::ToRadian(360) - acosf(direction.x);
			transform->Rotation(0, yRot + Math::ToRadian(90) + Math::ToRadian(180), 0);
		}
		else
		{
			//탐지 범위 벗어날 시 Path 이동
			if (distance > findRange)
				ChangeDirectionToPath(direction, i, position);
			//Move
			D3DXVec3Normalize(&direction, &direction);

			float yRot = -direction.z > 0 ? acosf(direction.x) : Math::ToRadian(360) - acosf(direction.x);

		
			//90도를 더 회전하는건 캐릭터가 z 방향을 보고 있기 때문 + 캐릭터가 -z방향을 기본으로 보기에 180도 꺾음
			transform->Rotation(0, yRot + Math::ToRadian(90) + Math::ToRadian(180), 0);
			transform->Position(position + -transform->Forward() * speed * Time::Delta());
			battacking = false;
			enemy->PlayClip(i, State::Walk);
		}
		
	}
}

void BowMaster::ArrowUpdate()
{
	
	//Transform* atransform = arrow->AddTransform();
	for (int i = 0; i < enemy->GetTransformCount(); i++)
	{
		if (a_attacks[i].Attacking() == false) continue;
		Transform* tr = arrow->GetTransform(i);
		tr->Position(tr->GetPosition() + -tr->Up() * 100 *Time::Delta());
		arrow->GetTransform(i)->Position(tr->GetPosition());

	}
	arrow->UpdateTransforms();
	arrow->Update();
}

void BowMaster::DrawAttack(UINT i, Transform * transform)
{
	//if (battacking == true) return;
	

	this->state = state;
	enemy->PlayClip(i, State::ShootArrow, 1.5, 1);

	
	
}

void BowMaster::Attack__(UINT i, Transform * transform)
{
	float attackTime = 0;
	float drawTime = 0;
	if (a_attacks[i].Attacking()) return;
	//if (battacking == false) return;
	//enemy->GetTransform(i)->Rotation(0, dir + Math::ToRadian(90) + Math::ToRadian(180), 0);
	attackTime = enemy->GetModel()->ClipByIndex(State::Draw)->Duration() / 30 * 900;
	drawTime = enemy->GetModel()->ClipByIndex(State::ShootArrow)->Duration() / 30 * 1000*1.5;
	enemy->PlayClip(i, State::Draw, 1.5,1);


	Transform* arrowTr = arrow->GetTransform(i);
	a_attacks[i].Start(attackTime, 2000, [=]
	{
		//arrowTr->Scale(1, 1, 1);
		arrowTr->Rotation(transform->GetRotation() + Vector3(Math::PI * 0.5f, 0, 0));
		arrowTr->Position(transform->GetPosition() + Vector3(0, 17, 0));
	}, [&, arrowTr] 
	{ 
		DisableArrow(arrowTr);  
	});
	arrow->GetTransform(i)->Rotation(arrowTr->GetRotation());
	arrow->GetTransform(i)->Position(arrowTr->GetPosition());

	atkTimer[i].Start([&, i]
	{
		ChangeState(i, State::Idle, 1, 1);
		//drawTimer[i].Start([&, i]
		//{
		//	ChangeState(i, State::Idle, 1, 1);
		//	//Attack__(i, enemy->GetTransform(i));
		//}, drawTime, 1);
		//battacking = false;
	}, attackTime, 1);

	arrow->UpdateTransforms();
}




vector<pair<function<bool()>, Render2D*>> BowMaster::GetUI()
{
	vector<pair<function<bool()>, Render2D*>> uis;

	for (int i = 0; i < hpRenders.size(); i++)
	{
		Render2D* render = hpRenders[i];
		uis.push_back(make_pair([=]()->bool
		{
			render->SRV(hpImg->SRV());
			return true;
		}, render));
	}

	return uis;
}

void BowMaster::DisableArrow(Transform * arrow)//화살을 안보이는 곳으로 보낸다.
{
	arrow->Position(0, -1000, 0);
}
void BowMaster::DisableEnemy(Transform * enemy)
{
	enemy->Position(0, 10000, 0);
}

void BowMaster::ChangeState(UINT i,State state, float speed, float takeTime)
{
	this->state = state;
	enemy->PlayClip(i, state, speed, takeTime);

}