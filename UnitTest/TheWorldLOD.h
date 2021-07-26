#pragma once
#include "Systems/IExecute.h"

struct EnemySetting
{
	UINT Type;
	UINT count = 1;
	bool Random = false;
	float x = 1200, z = 1000;
	float Hp = 40;
	float atk = 10;
	float defend = 0;
	bool Spawn = false;
};

struct LightSetting
{
	UINT Type;
	Color Ambient = Color(0,0,1,1);
	Color Diffuse = Color(0, 0, 1, 1);
	Color Specular = Color(0, 0, 1, 1);
	Color Emissive = Color(0, 0, 1, 1);
	Vector3 Position = Vector3(1200, 10, 1200);
	float Range = 30;

	Vector3 Direction = Vector3(0, -1, 0);
	float Angle = 30;

	float Intensity = 0.9;
	bool DrawDebug = true;
	bool Spawn = false;
};

class TheWorldLOD : public IExecute
{
private:
	typedef pair<function<bool()>, Render2D*>UI;

private:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	void Mesh();
	void Airplane();
	void KachujinA();
	void Pass(UINT mesh, UINT model, UINT anim);

	void AddPointLights(Color ambient, Color diffuse, Color Specular, Color Emissive, Vector3 Position, float Range, float intensity, bool debug);
	void AddPointLights();
	void AddSpotLights(Color ambient, Color diffuse, Color Specular, Color Emissive, Vector3 Position, float Range, Vector3 Direction, float Angle, float intensity, bool debug);
	void AddSpotLights();
private:
	void Player();
	void EZombie(UINT count, float x, float z, float Hp, float atk, float defend);
	void Archer(UINT count, float x, float z, float Hp, float atk, float defend);
	void SP();
	void AI_Pass(UINT val);
	void Erase();
	void Play(float x, float z, float Hp, float atk, float defend);
	void Spawn(UINT Type, UINT count, float x, float z, float Hp, float atk, float defend);
	void LightSpawn(UINT Type, Color ambient, Color diffuse, Color Specular, Color Emissive,Vector3 Position,float Range,Vector3 Direction,float Angle,float intensity,bool debug);
private:
	void OnGUI();
	void OnGUI_List();
	void OnGUI_Write();
	void OnGUI_Setttings();

	void UpdateParticleList();
	void UpdateTextureList();

	void WriteFile(wstring file);

	void SaveMap(wstring file);
	void OpenMap(wstring file);

	void OpenLayer(wstring name);
	void OpenLayer1(wstring name);
	void OpenLayer2(wstring name);

	wstring filePaths[4];

private:
	void SetBlur();

	void GetBlurParameter(vector<float>& weights, vector<Vector2>& offset, float x, float y);
	float GetGaussFunction(float val);

private:
	RenderTarget* renderTarget[8];
	DepthStencil* depthStencil;
	Viewport* viewport;

	float threshold = 0.9f;

	UINT blurCount = 15;
	float intensity = 3.5f;

	vector<float> weightX;
	vector<Vector2> offsetX;

	vector<float> weightY;
	vector<Vector2> offsetY;


	PostEffect* postEffect;
	Render2D* render2D;

private:
	EnemySetting enemyTypes;
	LightSetting lightSetting;
	Shader* terrainShader;
	Shader* shader;
	Shader* shader2D;
	class Sky* sky;

	class Terrain* terrain;
	//class TerrainRender* terrain;
	//class TerrainLod* terrain;
	class TerrainRender* Rterrain;
	class Shadow* shadow;
	class Water* lakeWater;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* sphere;
	MeshRender* cylinder;
	MeshRender* cube;
	MeshRender* grid;


	ModelRender* airplane = NULL;

	Model* weapon;
	ModelAnimator* kachujinA = NULL;

	vector<MeshRender *> meshes;
	vector<ModelRender *> models;
	vector<ModelAnimator *> animators;

private:
	wstring file = L"";
	class ParticleSystem* effectlight = NULL;
	class ParticleSystem* particle = NULL;
	UINT maxParticle = 0;

	vector<wstring> particleList;
	vector<wstring> textureList;

private:
	vector<UI> uis;//
//vector<UI> zuis;//
	class Hero* player = NULL;
	class BowMaster* bowMaster;
	class Zombie* zombie;
	int countbowm;

	vector<class ModelRender*>weapons;

	class Projector* projector;

	GBuffer* gBuffer;
	bool swordspawned = false;
	bool archerspawned = false;

	enum class WeatherType
	{
		None = 0, Rain, Snow
	} weatherType;

	class Rain* rain;
	class Snow* snow;
	bool bLookGbuffer;

	class Fixity* camera;
	Perspective* perspective;
	Frustum* frustum;

	float fov = 0.45f;
	float zFar = 750.0f;

	float terrainscale = 1;
};
