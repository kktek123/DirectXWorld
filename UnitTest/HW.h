#pragma once
#include "Systems/IExecute.h"

class HW :public IExecute
{

private:
	typedef pair<function<bool()>, Render2D*>UI;

private:
	virtual void Initialize() override;
	virtual void Ready() {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() {};
	virtual void Render() override;
	virtual void PostRender() {};
	virtual void ResizeScreen() {};

private:
	Shader* shader;
	Shader* shader2D;


	Material* floor;
	MeshRender* grid;
	vector<UI> uis;//
	//vector<UI> zuis;//
	class Hero* player;
	class BowMaster* bowMaster;
	class Zombie* zombie;
	int countbowm;

	vector<class ModelRender*>weapons;

	class Projector* projector;

	class CubeSky* sky;

	void Mesh();


};