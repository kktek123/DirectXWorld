#include "stdafx.h"
#include "Zombie.h"
#include "Hero.h"
#include "Viewer/Freedom.h"
#include "Environment/Terrain.h"
#include "Model/ModelMesh.h"
#include "Model/ModelAnimator.h"
#include "Model/ModelClip.h"
#include "Model/Model.h"


Zombie::Zombie(Shader* shader, Shader* val, Transform* tr)
	:shader(shader), shader2D(val)
{
	//ctransform = new Transform();
	Weapons();
	ZombieEnemy();
	hpImg = new Texture(L"Red.png");
	
	//pass = val;
}
Zombie::~Zombie()
{
	SafeDelete(enemy);

	for (int i = 0; i < hitColliders.size(); i++)
		SafeDelete(hitColliders[i]);

	for (int i = 0; i < attackColliders.size(); i++)
		SafeDelete(attackColliders[i]);

	//SafeDelete(arrow);
}

void Zombie::Update(Terrain* terrain)
{
	Move(terrain);
	//ArrowUpdate();
	for (UINT i = 0; i < attackColliders.size(); i++)
	{
		//attackColliders[i]->GetTransform()->World(enemy->GetAttach(i, 35));//무기 충돌체 위치
		attackColliders[i]->Update();
	}
	for (UINT i = 0; i < hitColliders.size(); i++)
	{
		//hitColliders[i]->GetTransform()->World(enemy->GetAttach(i, 0));
		//hitColliders[i]->GetTransform()->Scale(2.f, 4.0f, 2.0f);
		hitColliders[i]->Update();
	}

	if (enemy != NULL)
	{
		enemy->Update();
		//enemy->ComputeUpdate();
		for (UINT i = 0; i < attackColliders.size(); i++)
		{
			attackColliders[i]->GetTransform()->World(enemy->GetBoneAnimationMatrix(i, 35));
		}
		enemy->UpdateTransforms();
	}

	//for (UINT i = 0; i < hitColliders.size(); i++)
	//{
	//	hitColliders[i]->Update();
	//}

	
	for (int i = 0; i < hpRenders.size(); i++)
	{
		Render2D* hpRender = hpRenders[i];
		Vector3 position;
		Context::Get()->GetViewport()->Project(&position, hpPosition, enemy->GetTransform(i)->World(), Context::Get()->View(), Context::Get()->Projection());
		position.y = D3D::Height() - position.y;

		if (position > 0 && position.z < 1)
		{
			position.z = 0;
		}
		Vector3 scale = hpRender->GetTransform()->GetScale();
		scale.x = hp[i] * (200.0f / 40.0f);//길이 / 최대 체력 = 체력 1당 길이
		scale.y = hp[i] * 0.1f;
		position.x -= 100;
		position.x += scale.x*0.5f;

		hpRender->GetTransform()->Scale(scale);
		hpRender->GetTransform()->Position(position);
		hpRender->Update();
	}
}

void Zombie::Render()
{

	if (enemy != NULL) enemy->Render();
	//arrow->Render();
	//for (UINT i = 0; i < hitColliders.size(); i++)
	//{
	//	if (!dead[i])
	//		hitColliders[i]->Render(Color(0, 1, 0, 1));
	//}
	//for (UINT i = 0; i < attackColliders.size(); i++)
	//{
	//	attackColliders[i]->Render(Color(0, 1, 0, 1));
	//}
}


Transform * Zombie::AddTransform(float health = 100, float attack = 40, float defend = 0)
{
	Transform* tr = enemy->AddTransform();
	
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
		Transform* sTr = new Transform();
		//Transform* arrawTr = arrow->AddTransform();

		cTr->SetShader(shader);
		//cTr->Position(0, 100, 0);
		//cTr->Position(-90.f, 160, 10);
		//cTr->Scale(10, 10, 10);
		cTr->Position(-80.0, 160.0, -50.0);
		cTr->Scale(30.f, 30.f, 80.f);
		Collider* collider = new Collider(sTr,cTr);
		//collider = new Collider(arrawTr);
		//collider->GetTransform()->SetShader(shader);
		//collider->GetTransform()->Scale(2, 16, 2);
		//DisableArrow(arrawTr);

		attackColliders.push_back(collider);
		atkTimer.push_back(Timer());
		a_attacks.push_back(AttackTimer());
	}
	//Hp setting
	{
		Render2D* hpRender = new Render2D(shader2D,0);
		//hpRender->Pass(pass);
		hpRender->GetTransform()->Scale(300, 20, 1);
		hp.push_back(health);
		hpRenders.push_back(hpRender);

		dead.push_back(false);
	}
	//atk,def setting
	{
		patroltimer.push_back(0);
		atk.push_back(attack);
		def.push_back(defend);
		
	}

	return tr;
}

void Zombie::InitTransforms()
{
	enemy->UpdateTransforms();
}

void Zombie::Destroy()
{
	//enemy->EraseTransform();

}



void Zombie::Weapons()
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


void Zombie::Pass(UINT val)
{
	enemy->Pass(val);
}


void Zombie::ZombieEnemy()
{
	enemy = new ModelAnimator(shader);
	enemy->ReadMaterial(L"Enemy/Mesh");
	enemy->ReadMesh(L"Enemy/Mesh");
	enemy->ReadClip(L"Enemy/Idle");
	enemy->ReadClip(L"Enemy/Running");
	enemy->ReadClip(L"Enemy/Attack1");
	enemy->ReadClip(L"Enemy/Hit");
	enemy->ReadClip(L"Enemy/Death");

	//Enemys etransform;

	Transform attachTransform;


	////Sword
	//attachTransform.Position(-0, -0, -0);
	//attachTransform.Scale(1.0f, 1.0f, 1.0f);
	//attachTransform.RotationDegree(0, 0, 90);

	attachTransform.Position(0, 10, -10);
	attachTransform.Scale(1.f, 1.f, 1.f);

	attachTransform.RotationDegree(0, 0, 90);

	////LongBow



	enemy->GetModel()->Attach(shader, weapons[6], 35, &attachTransform);
	//enemy->Pass(2);

	//enemy->UpdateTransforms();



}
void Zombie::Attack(Hero * player)
{
	if (player->CanHurt() == false) return;

	for (int i = 0; i < attackColliders.size(); i++)
	{
		if (a_attacks[i].Attacking() == false) continue;

		//맞음
		if (attackColliders[i]->IsIntersect(player->GetHurt()) && player->bImotal == false)
		{
			atkTimer[i].Stop();

			//if (weaponType == WeaponType::Bow)
			//DisableArrow(arrow->GetTransform(i));

			player->Damage(atk[i]);

			return;
		}
	}
}
void Zombie::Damage(UINT i, float Damage)
{
	if (hitTimer[i].Complete() == false || dead[i]) return;
	hp[i] -= (Damage - def[i] > 0) ? (Damage - def[i]) : 0;
	if (hp[i] <= 0 && i>=0)
	{
		dead[i] = true;
		enemy->PlayClip(i, State::Die, 1, 1);

		hitTimer[i].Start([&, i]
		{
			enemy->Stop(i);
			enemy->EraseTransform(i);
			hitColliders.erase(hitColliders.begin() + i);
			attackColliders.erase(attackColliders.begin() + i);
			hp.erase(hp.begin() + i);
			atk.erase(atk.begin() + i);
			def.erase(def.begin() + i);
			//atkTimer.erase(atkTimer.begin() + i);
			//hitTimer.erase(hitTimer.begin() + i);
			//a_attacks.erase(a_attacks.begin() + i);
			hpRenders.erase(hpRenders.begin() + i);
			dead.erase(dead.begin() + i);


		}, enemy->GetModel()->ClipByIndex(State::Die)->Duration() / 30 * 1000 - 1, 1);
	}
	else
	{
		enemy->PlayClip(i, State::Reaction, 1, 1);

		hitTimer[i].Start([&, i]
		{
			enemy->PlayClip(i, State::Idle, 1, 1);
		}, enemy->GetModel()->ClipByIndex(State::Reaction)->Duration() / 30 * 1000, 1);

	}
}

void Zombie::CreateRandomPath(float range, UINT count)
{
	pathCount = count;

	for (int i = 0; i < enemy->GetTransformCount(); ++i)
	{
		Vector3 pos; enemy->GetTransform(i)->Position(&pos);

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
			//positions[j].x = Math::SRandom(pos.x - range, pos.x + range);
			//positions[j].y = enemy->GetTransform(i)->GetPosition().y;
			//positions[j].z = Math::SRandom(pos.z - range, pos.z + range);
		}

		//enemy->GetTransform(i)->Position(positions[0]);

		paths.push_back(positions);
		nextPathNums.push_back(1);
	}
}

void Zombie::ChangeDirectionToPath(Vector3 & direction, UINT i, Vector3 & position)
{
	direction = paths[i][nextPathNums[i]] - position;

	//D3DXVec3Length(&direction);
	if (D3DXVec3Length(&direction) <=  10.3f )
	{
		//ChangeState(i, State::Idle, 1, 1);
		enemy->PlayClip(i, State::Idle, 1, 1);
		speed = 0;
		//nextPathNums[i]++;
		//if (nextPathNums[i] >= pathCount)
		//	nextPathNums[i] = 0;
		//direction = paths[i][nextPathNums[i]] - position;
		//ChangeDirectionToPath(direction, i, position);
	}
}

Transform * Zombie::GetNearInstance(Vector3 position, float * outDistance)//플레이어 타겟
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

void Zombie::Move(Terrain* terrain)
{
	Vector3 targetPosition;
	if (target != NULL)
		target->Position(&targetPosition);

	for (int i = 0; i < enemy->GetTransformCount(); ++i)
	{
		//if (target != NULL) continue;
		if (dead[i] || atkTimer[i].Complete() == false || hitTimer[i].Complete() == false) continue;

		transform = enemy->GetTransform(i);
		Vector3 position;
		transform->Position(&position);
		Vector3 direction = targetPosition - position;
		float distance = D3DXVec3Length(&direction);
		position.y = terrain->GetHeight(position);
		//Attack
		if (distance < attackRange)
		{
			Attack__(i, transform);
		}
		else
		{
			if (distance <= findRange)
			{
				speed = 200;
				enemy->PlayClip(i, State::Walk, 1, 1);

			}

			//탐지 범위 벗어날 시 Path 이동
			if (distance > findRange)
			{
				ChangeDirectionToPath(direction, i, position);
				//patroltimer[i]++;
				//if (patroltimer[i] >= 1000)
				//{
				//	patroltimer[i] = 0;
				//	patroltimer[i]++;
				//	float r = (rand() % 360) / 180.0f*3.14;
				//	direction.y = atan(r);
				//}
				//enemy->PlayClip(i, State::Walk,1,1);

			}

		}
			//Move
		D3DXVec3Normalize(&direction, &direction);

		float yRot = -direction.z > 0 ? acosf(direction.x) : Math::ToRadian(360) - acosf(direction.x);

		//90도를 더 회전하는건 캐릭터가 z 방향을 보고 있기 때문 + 캐릭터가 -z방향을 기본으로 보기에 180도 꺾음
		transform->Rotation(0, yRot + Math::ToRadian(90) + Math::ToRadian(180), 0);
		transform->Position(position + -transform->Forward() * speed * Time::Delta());

		
	}
}

void Zombie::ArrowUpdate()
{
	//for (int i = 0; i < enemy->GetTransformCount(); i++)
	//{
	//	if (a_attacks[i].Attacking() == false) continue;

	//	Transform* tr = arrow->GetTransform(i);
	//	tr->Position(tr->GetPosition() + -tr->Up() * 100 * Time::Delta());
	//}

	//arrow->UpdateTransforms();
	//arrow->Update();
}

void Zombie::Attack__(UINT i, Transform * transform)
{
	speed = 200;
	float attackTime = 0;
	if (a_attacks[i].Attacking()) return;

	enemy->PlayClip(i, State::Attack_);
	attackTime = enemy->GetModel()->ClipByIndex(State::Attack_)->Duration() / 30 * 1000;

	//Transform* arrowTr = arrow->GetTransform(i);
	a_attacks[i].Start(attackTime, 4000);

	atkTimer[i].Start([&, i]
	{
		//ChangeState(i, State::Idle,1,1);
	}, attackTime, 1);
}





vector<pair<function<bool()>, Render2D*>> Zombie::GetUI()
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

void Zombie::ChangeState(UINT i, State state, float speed, float takeTime)
{
	this->state = state;
	enemy->PlayClip(i, state, speed, takeTime);
}

void Zombie::DisableArrow(Transform * arrow)//화살을 안보이는 곳으로 보낸다.
{
	arrow->Position(0, 10000, 0);
}
void Zombie::DisableEnemy(Transform * enemy)
{
	enemy->Position(0, 10000, 0);
}


//void Zombie::EquipWeapon(UINT i)
//{
//	Matrix currBoneMatA = enemy->GetCurrboneTransA(0, 35);
//	//Matrix currBoneMatB = kachujin->GetCurrboneTransB(0, 35);
//
//
//	Matrix currClipMat;
//	Matrix nextClipMat;
//	Matrix S, R, T;
//
//
//
//	Math::LerpMatrix(currClipMat, currBoneMatA, currBoneMatA, enemy->GetCurrTime(0));
//
//	if (enemy->GetNextClip(0) > -1)
//	{
//		Matrix nextBoneMatA = enemy->GetNextboneTransA(0, 35);
//		//Matrix nextBoneMatB = kachujin->GetNextboneTransB(0, 35);
//		Math::LerpMatrix(nextClipMat, nextBoneMatA, nextBoneMatA, enemy->GetCurrTime(0));
//		Math::LerpMatrix(currClipMat, currClipMat, nextClipMat, enemy->GetNextTime(0));
//	}
//	wtransform->World(currClipMat);
//
//}