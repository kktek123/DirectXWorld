#pragma once
class Hero 
{
public:
	enum State
	{
		Idle,
		Walk,
		Run,
		Hit,
		Death,
		Attack1,
		Attack2,
		Attack3,
		powerup,
		spatk,
	};
public:
	Hero(Shader* shader, Shader* val);
	~Hero();
	void Update();
	void Update(class Terrain* terrain);
	void Render();
	void ParticleRender();

	Transform* GetTransform() { return transform; }
	Collider* GetHit() { return Swordcol; }
	Collider* GetHurt() { return Playercol; }

	bool GetDamage() { return hitTimer.Complete(); }
	bool GetAttack() { return attacking; }
	bool CanHurt() { return hitTimer.Complete(); }
	vector<pair<function<bool()>, Render2D*>> GetUI();


	void Attack(class BowMaster* enemy);
	void Attack(class Zombie* enemy);

	void Damage(float damage);

	void SetNearEnemy(Transform* enemyTr, float distance) { nearEnemy = enemyTr; nearEnemyDistance = distance; }
	bool Getspatk() { return specialAttack; }
	void Pass(UINT val);
	ModelAnimator* GetPawn() { return kachujin; }
	bool Getdead() { return state == State::Death; }
private:
	void Weapons();
	void Kachujin();

	void KachujinAI(class Terrain* terrain);// class Terrain* terrain);

	void Move(class Terrain* terrain);
	void Action();
	void UIUpdate();
	void GetItem();

	void ChangeState(State state, float speed = 1, float takeTime = 1);

	Vector3 GetDirection(State* moveState);

private:
	class ParticleSystem* particle = NULL;
	Shader* shader;
	//Shader* magicShader;
	//class Projector* projector;
	
	Model* weapons[11];

	ModelAnimator* kachujin = NULL;

	Timer hitTimer , attackTimer,dashAttackTimer;
	AttackTimer a_attack;
	bool attacking = false;
	bool Out = false;

	Collider* collider;
	Collider* Swordcol = NULL;
	vector<Collider*> colliders;
	Transform* mtransform = NULL;
	Transform* ctransform = NULL;
	Transform* wtransform = NULL;
	Transform* stransform = NULL;

	Transform* transform = NULL;

	vector<D3DXVECTOR3*> Dirs;
	//Vector3 Dir[3];
	Vector3 Rot[3];



	State state = State::Idle;
	float speed;

	float hp = 100;
	Texture* hpImg;
	Render2D* hpRender;

	float nearEnemyDistance;
	Transform* nearEnemy;

	
	//Vector3 mposition;// , mrotation = NULL;
	Vector3 mrotation;
	Collider* Playercol;
	Vector3 mposition;
	Vector3 direction;

	bool specialAttack = false;
	bool bpowerup = false;


public:
	bool bImotal = true;
};
