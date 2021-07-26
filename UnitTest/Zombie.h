#pragma once

class Zombie
{
public:
	enum State
	{
		Idle,
		Walk,
		Attack_,
		Reaction, //Impact, hurt
		Die,
	};

private:

public:
	Zombie(Shader* shader, Shader* val, Transform* tr = NULL);
	~Zombie();
	void Update(Terrain* terrain);
	//void AI(Vector3 transform, class Terrain* terrain, bool hit, Collider* swordcol);
	void Render();
	Transform* AddTransform(float hp, float atk, float def);
	void InitTransforms();
	void Destroy();

	void Attack(class Hero* player);
	void Damage(UINT i, float Damage);
	void CreateRandomPath(float range, UINT count);
	bool CanImpact(UINT instanceID) { return hitTimer[instanceID].Complete(); }
	bool IsDead(UINT instanceID) { return dead[instanceID]; }


	UINT GetCount() { return enemy->GetTransformCount(); }
	Collider* GetHurtCollider(UINT instanceID) { return hitColliders[instanceID]; }
	Transform* GetNearInstance(Vector3 position, float* outDistance);
	vector<pair<function<bool()>, Render2D*>> GetUI();

	void SetFindRange(float value) { findRange = value; }
	void SetAttackRange(float value) { attackRange = value; }
	void SetTraget(Transform* target) { this->target = target; }
	void SetSpeed(float value) { speed = value; }

	void Pass(UINT val);
	ModelAnimator* GetPawn() { return enemy; }
	void EquipWeapon(UINT i);

private:

	void ZombieEnemy();
	void Weapons();
private:
	State state = State::Idle;

	//	vector<ModelRender*>* weapons;

	Shader* shader;
	Shader* shader2D;

	//Enemys enemys[100];

	Model* weapons[11];

	ModelAnimator* enemy = NULL;

	vector<Collider*> colliders;
	vector<Collider*> attackColliders, hitColliders;
	ModelRender* arrow = NULL;

	vector<Timer> atkTimer, hitTimer;
	vector<AttackTimer> a_attacks;
	vector<int> patroltimer;

	float speed = 200;
	float findRange = 0;
	float attackRange = 0;

	Texture* hpImg;
	vector<float> hp, atk, def;
	vector<Render2D*> hpRenders;
	Vector3 hpPosition = Vector3(0, 200, 0);
	vector<bool> dead;

	void ChangeState(UINT i, State state, float speed, float takeTime);

	//vector<Enemys> BowMasters;
	//Transform* ctransform = NULL;
	//Transform* wtransform = NULL;


	vector<D3DXVECTOR3*> Dirs;



	vector<Vector3*> paths;
	vector<UINT> nextPathNums;
	UINT pathCount = 0;
	//Vector3 Dir[3];
	Vector3 Rot[3];
	UINT pass;

	//Vector3 eposition[100];

	Collider* Enemycol[100];
	void DisableArrow(Transform* arrow);
	void DisableEnemy(Transform * enemy);

public:

private:
	void Move(Terrain* terrain);
	void ArrowUpdate();

	void Attack__(UINT i, Transform* transform);
	void ChangeDirectionToPath(Vector3& direction, UINT i, Vector3& position);

	Transform* target = NULL;
	Transform* transform;

	

};
