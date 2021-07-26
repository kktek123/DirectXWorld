#include "stdafx.h"
#include "TheWorldLOD.h"
#include "Utilities/Path.h"
#include "Utilities/Xml.h"
#include "Hero.h"
#include "Zombie.h"
#include "BowMaster.h"
#include "Environment/TerrainRender.h"


void TheWorldLOD::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(1000, 29, 1000);
	
	((Freedom *)Context::Get()->GetCamera())->Speed(100, 2);

	shader = new Shader(L"050_TheWorldLOD2.fx");
	shader2D = new Shader(L"29_Render2D.fx");
	//for (int i = 0; i < 3; i++)
	//{
	//	int n = i * 3;
	//	shadow[i * 3] = new Shadow(shader, Vector3(-100 + 100*i, 0,-100), 100);
	//	shadow[i * 3 + 1] = new Shadow(shader, Vector3(-100 + 100 * i, 0, 0), 100);
	//	shadow[i * 3 + 2] = new Shadow(shader, Vector3(-100 + 100 * i, 0, 100), 100);
	//}
	shadow = new Shadow(shader, Vector3(1000, 0, 1000), 500,2048, 2048);
	sky = new Sky(shader);
	sky->ScatteringPass(6);
	sky->RealTime(false, Math::PI - 1e-6f);
	lakeWater = new Water(shader, 10, 5000, 5000);
	lakeWater->GetTransform()->Position(100, 10, 100);
	lakeWater->GetTransform()->Scale(500, 3, 500);
	gBuffer = new GBuffer(shader);


	rain = new Rain(Vector3(300, 100, 500), (UINT)1e+4, L"Environment/Rain.png");
	snow = new Snow(Vector3(300, 100, 500), (UINT)1e+5, L"Environment/Snow.png");

	terrain = new Terrain(shader, L"Terrain/Gray512.png");
	//terrain->BaseMap(L"Terrain/Dirt3.png");
	//terrain->LayerMap(L"Terrain/Grass (Lawn).jpg", L"Terrain/Gray512.png");
	//terrain->NormalMap(L"Terrain/Dirt_Normal.png");
	terrain->BaseMap(L"Terrain/rock01d.tga");
	//terrain->LayerMap(L"Terrain/Grass (Lawn).jpg");
	//terrain->AlphaMap( L"Terrain/hm.bmp");
	//terrain->LayerMap(L"Terrain/Snow.jpg", L"Terrain/Splatting.png");
	//terrain->NormalMap(L"Terrain/Dirt3_Normal.png");//rock01n.tga
	terrain->NormalMap(L"Terrain/rock01n.tga");//rock01n.tga
	//terrain->DetailMap(L"Terrain/detail001.dds");
	terrain->SpecularMap(L"Terrain/Dirt3_Specular.png");
	//terrain->BaseMap(L"Terrain/dirt01.dds");
	//terrain->NormalMap(L"Terrain/dirt01.dds");
	terrain->DetailMap(L"Terrain/detail001.dds");
	//terrain->DetailMap(L"Terrain/rock01d.tga");
	//terrain->SnowMap(L"Terrain/Snow.jpg");
	terrain->SnowMap(L"Terrain/snow01n.tga");
	terrain->DistanceMap(L"Terrain/distance01n.tga");
	terrain->AlphaMap(L"Terrain/Gray512.png");

	terrainscale = 4;

	terrain->GetTransform()->Position(0, 0, 0);
	terrain->GetTransform()->Scale(terrainscale, 1.7, terrainscale);
	//{
	//	Transform* transform = NULL;
	//	terrain = new TerrainRender(shader, pterrain);
	//	transform = terrain->AddTransform();
	//	transform->Position(-200, 0, -200);
	//	transform->Scale(3, 1, 3);
	//	terrain->UpdateTransforms();
	//}
	
	Mesh();
	Airplane();
	//KachujinA();

	UpdateParticleList();
	UpdateTextureList();
	effectlight = new ParticleSystem(L"Explosion");
	//projector = new Projector(shader, L"Environment/MagicCircle.png", 100, 100);
	//Player();

	//Archer(3, 200, 200, 40, 0, 0);
	//archerspawned = true;
	//EZombie(3, 200, 200, 40, 10, 0);
	//EZombie();
	//Archer();
	//AddPointLights();
	//AddSpotLights();

	for (UINT i = 0; i < 8; i++)
		renderTarget[i] = new RenderTarget();

	depthStencil = new DepthStencil();
	viewport = new Viewport(D3D::Width(), D3D::Height());

	postEffect = new PostEffect(L"54_Bloom.fx", gBuffer->Srvs());

	render2D = new Render2D();
	render2D->GetTransform()->Position(200, 120, 0);
	render2D->GetTransform()->Scale(355.5f, 200, 1);
	render2D->SRV(renderTarget[0]->SRV());

	camera = new Fixity();
	camera->Position(0, 0, -50);
	perspective = new Perspective(1024, 1024, 1, zFar, Math::PI * 0.5);

	//frustum = new Frustum(camera, perspective);
	frustum = new Frustum(NULL, perspective);

}

void TheWorldLOD::Destroy()
{
	
}

void TheWorldLOD::Update()
{
	
	OnGUI();

	ImGui::InputFloat("Far", &zFar, 1.0f);
	ImGui::InputFloat("Fov", &fov, 1e-3f);
	perspective->Set(1024, 768, 1, zFar, Math::PI * fov);

	frustum->Update();


	ImGui::SliderFloat("Threshold", &threshold, 0, 1);
	postEffect->GetShader()->AsScalar("Threshold")->SetFloat(threshold);

	ImGui::InputInt("BlurCount", (int *)&blurCount, 2);
	blurCount %= 33;
	if (blurCount < 1)
		blurCount = 1;
	postEffect->GetShader()->AsScalar("SampleCount")->SetInt(blurCount);


	ImGui::InputFloat("BlurIntensity", &intensity, 0.01f);
	postEffect->GetShader()->AsScalar("Intensity")->SetFloat(intensity);

	UINT selected = (UINT)weatherType;
	ImGui::Separator();
	ImGui::InputInt("Weather", (int *)&selected);
	selected %= 3;
	weatherType = (WeatherType)selected;

	switch (weatherType)
	{
	case WeatherType::Rain: rain->Update(); break;
	case WeatherType::Snow: snow->Update(); break;
	}

	ImGui::Checkbox("DrawGbuffer", &bLookGbuffer);
	terrain->Update();

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Enemy Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Play"))
		{
			Play(enemyTypes.x, enemyTypes.z, enemyTypes.Hp, enemyTypes.atk, enemyTypes.defend);

		}
		//ImGui::ColorEdit3("FogColor", Lights::Get()->FogColor());
		ImGui::InputInt("EnemyType", (int *)&enemyTypes.Type);
		enemyTypes.Type %= 3;

		ImGui::InputInt("Count", (int *)&enemyTypes.count, 1);
		enemyTypes.count %= 20;

		ImGui::SliderFloat("X", &enemyTypes.x, 0, 3000.0f);
		enemyTypes.count %= 500;
		ImGui::SliderFloat("Z", &enemyTypes.z,0, 3000.0f);
		enemyTypes.count %= 500;

		ImGui::Checkbox("Random", &enemyTypes.Random);

		ImGui::SliderFloat("Hp", &enemyTypes.Hp,10.0f,300.0f);
		ImGui::SliderFloat("Attack", &enemyTypes.atk, 10.0f, 300.0f);
		ImGui::SliderFloat("Defend", &enemyTypes.defend, 10.0f, 300.0f);
		ImGui::Checkbox("EnemySpawnMouse", &enemyTypes.Spawn);
		if (ImGui::Button("EnemySpawn"))
		{
			Spawn(enemyTypes.Type, enemyTypes.count, enemyTypes.x, enemyTypes.z, enemyTypes.Hp, enemyTypes.atk, enemyTypes.defend);
		}
	

		if (Mouse::Get()->Down(0) && Keyboard::Get()->Press(VK_SPACE) && enemyTypes.Spawn)
		{
			Vector3 position = terrain->GetPickedPosition();
			Spawn(enemyTypes.Type, enemyTypes.count, position.x*terrainscale, position.z*terrainscale, enemyTypes.Hp, enemyTypes.atk, enemyTypes.defend);
		}

	}
	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//ImGui::ColorEdit3("FogColor", Lights::Get()->FogColor());
		ImGui::InputInt("lightType", (int *)&lightSetting.Type);
		lightSetting.Type %= 3;

		ImGui::Checkbox("DrawDebug", &lightSetting.DrawDebug);
		gBuffer->DrawDebug(lightSetting.DrawDebug);

		ImGui::ColorEdit3("Ambient", lightSetting.Ambient);			
		ImGui::ColorEdit3("Diffuse", lightSetting.Diffuse);				
		ImGui::ColorEdit3("Specular", lightSetting.Specular);				
		ImGui::ColorEdit3("Emissive", lightSetting.Emissive);

		ImGui::SliderFloat3("Position", lightSetting.Position,0,3000);

		ImGui::InputFloat("lightRange", &lightSetting.Range);

		ImGui::InputFloat("Intensity", &lightSetting.Intensity);

		ImGui::SliderFloat3("Direction", lightSetting.Direction,-1,1);

		ImGui::InputFloat("Angle", &lightSetting.Angle);

		ImGui::Checkbox("LightSpawnMouse", &enemyTypes.Spawn);

		if (ImGui::Button("LightSpawn"))
		{
			
			LightSpawn(lightSetting.Type, lightSetting.Ambient, lightSetting.Diffuse, lightSetting.Specular, lightSetting.Emissive, lightSetting.Position, lightSetting.Range, lightSetting.Direction, lightSetting.Angle, lightSetting.Intensity, lightSetting.DrawDebug);
		}
	}
	


	//UINT& type = Lights::Get()->FogType();
	//if (ImGui::CollapsingHeader("Fog", ImGuiTreeNodeFlags_DefaultOpen))
	//{
	//	ImGui::InputInt("FogType", (int*)&type);
	//	type %= 4;
	//	ImGui::ColorEdit3("FogColor", Lights::Get()->FogColor());
	//	ImGui::SliderFloat("FogDensity", &Lights::Get()->FogDensity(), 0, 1, "% 2f");
	//	ImGui::SliderFloat("FogDistancX", &Lights::Get()->FogDistance().x, 0, 10.0f);
	//	ImGui::SliderFloat("FogDistancY", &Lights::Get()->FogDistance().y, 0, 300.0f);

	//}


	Vector3 position;
	sphere->GetTransform(0)->Position(&position);

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

	sphere->GetTransform(0)->Position(position);
	sphere->UpdateTransforms();


	sky->Update();

	sphere->Update();
	grid->Update();
	

	if (airplane != NULL) airplane->Update();
	if (kachujinA != NULL) kachujinA->Update();


	lakeWater->Update();
	


	if (particle != NULL)
	{
		particle->Add(position);
		particle->Update();
	}

	SP();

	postEffect->Update();
	render2D->Update();

}

void TheWorldLOD::PreRender()
{
	sky->PreRender();
	//Shadow Depth
	{
		//for (int i = 0; i < 9; i++)
		//{
		//	shadow[i]->PreRender();

		//}
		shadow->PreRender();
		//terrain->Pass(2);
		//terrain->Render();
		Pass(3, 4, 5);
		floor->Render();
		grid->Render();
		brick->Render();
		sphere->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujinA != NULL) kachujinA->Render();

		if (bowMaster != NULL)bowMaster->Render();
		if (zombie != NULL)zombie->Render();
		if (player != NULL) player->Render();
	}

	//gBuffer->PreRender();


	gBuffer->PreRender();
	{
		

		sky->Pass(7, 8, 9);
		sky->Render();

		terrain->Pass(1);
		terrain->Render();
		Pass(16, 17, 18);
		floor->Render();
		grid->Render();
		brick->Render();
		sphere->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujinA != NULL) kachujinA->Render();

		if (bowMaster != NULL)bowMaster->Render();
		if (zombie != NULL) zombie->Render();
		if (player != NULL) player->Render();
		
		lakeWater->Pass(19);
		lakeWater->Render();

	}
	{
		lakeWater->PreRender_Reflection();
		sky->Pass(13, 14, 15);
		sky->Render();
		terrain->Pass(0);
		terrain->Render();
		Pass(10, 11, 12);
		floor->Render();
		grid->Render();
		brick->Render();
		sphere->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujinA != NULL) kachujinA->Render();

		if (bowMaster != NULL)bowMaster->Render();
		if (zombie != NULL) zombie->Render();
		if (player != NULL) player->Render();
	}
	//Refraction PreRender
	{
		lakeWater->PreRender_Refraction();

		sky->Pass(7, 8, 9);
		sky->Render();

		terrain->Pass(1);
		terrain->Render();
		Pass(25, 26, 27);
		floor->Render();
		grid->Render();
		brick->Render();
		sphere->Render();
		if (airplane != NULL) airplane->Render();
		if (kachujinA != NULL) kachujinA->Render();

		if (bowMaster != NULL)bowMaster->Render();
		if (zombie != NULL) zombie->Render();
		if (player != NULL) player->Render();
	}

	//Main Render
	{
		renderTarget[0]->Set(depthStencil);
		viewport->RSSetViewport();

		sky->Pass(7, 8, 9);
		sky->Render();

		lakeWater->ResetClipPlane();

		Plane planes[4];
		frustum->Planes(planes);

		shader->AsVector("CullPlane")->SetFloatVectorArray((float *)planes, 0, 4);

		terrain->Pass(1);
		terrain->Render();
		//lakeWater->Pass(19);
		//lakeWater->Render();
		gBuffer->Render(renderTarget[0]);

		if (particle != NULL)
			particle->Render();
		if (player != NULL) player->ParticleRender();

		switch (weatherType)
		{
		case WeatherType::Rain: rain->Render(); break;
		case WeatherType::Snow: snow->Render(); break;
		}
	}

	//Luminosity
	{
		renderTarget[1]->Set(depthStencil);
		viewport->RSSetViewport();

		postEffect->Srv(0, renderTarget[0]->SRV());
		postEffect->Pass(0);
		postEffect->Render();
	}


	SetBlur();

	//BlurX
	{
		postEffect->GetShader()->AsScalar("Weights")->SetFloatArray(&weightX[0], 0, weightX.size());
		postEffect->GetShader()->AsVector("Offsets")->SetRawValue(&offsetX[0], 0, sizeof(Vector2) * offsetX.size());

		renderTarget[2]->Set(depthStencil);
		viewport->RSSetViewport();

		postEffect->Srv(1, renderTarget[1]->SRV());
		postEffect->Pass(1);
		postEffect->Render();
	}

	//BlurY
	{
		postEffect->GetShader()->AsScalar("Weights")->SetFloatArray(&weightY[0], 0, weightY.size());
		postEffect->GetShader()->AsVector("Offsets")->SetRawValue(&offsetY[0], 0, sizeof(Vector2) * offsetY.size());

		renderTarget[3]->Set(depthStencil);
		viewport->RSSetViewport();

		postEffect->Srv(2, renderTarget[2]->SRV());
		postEffect->Pass(2);
		postEffect->Render();
	}

	//Composite
	{
		renderTarget[4]->Set(depthStencil);
		viewport->RSSetViewport();

		postEffect->Srv(3, renderTarget[3]->SRV());
		postEffect->Pass(3);
		postEffect->Render();
	}

	//Color Grading
	{
		renderTarget[5]->Set(depthStencil);
		viewport->RSSetViewport();

		postEffect->Srv(4, renderTarget[4]->SRV());
		postEffect->Pass(4);
		postEffect->Render();
	}
}


void TheWorldLOD::Render()
{
	postEffect->Srv(5, renderTarget[5]->SRV());
	postEffect->Pass(5);

	postEffect->Render();
	//render2D->Render();

	//
	//sky->Pass(7, 8, 9);
	//sky->Render();
	////gBuffer->Render();
	//	//Reflection PreRender

	//lakeWater->ResetClipPlane();
	////lakeWater->Pass(28);
	////lakeWater->Render();
	//
	//gBuffer->Render();
	////terrain->Pass(1);
	////terrain->Render();
	////Pass(16, 17, 18);
	////floor->Render();
	////grid->Render();
	////brick->Render();
	////sphere->Render();
	////if (airplane != NULL)
	////	airplane->Render();
	////if (kachujinA != NULL)
	////	kachujinA->Render();


	////projector->Render();

	////if(zombie != NULL)zombie->Render();
	////if (player != NULL) player->Render();
	//////if (bowMaster != NULL) bowMaster->Render();

	//if (particle != NULL)
	//	particle->Render();
	//if (player != NULL) player->ParticleRender();

	//switch (weatherType)
	//{
	//case WeatherType::Rain: rain->Render(); break;
	//case WeatherType::Snow: snow->Render(); break;
	//}

	for (UI& ui : uis)
	{
		if (ui.first())
			ui.second->Render();
	}
	
}

void TheWorldLOD::PostRender()
{
	if (bLookGbuffer)
	{
		gBuffer->PostRender();
	}
	//gBuffer->PostRender();
}

void TheWorldLOD::Mesh()
{
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->SpecularMap("Floor_Specular.png");
	floor->NormalMap("Floor_Normal.png");
	floor->Specular(1, 1, 1, 20);

	brick = new Material(shader);
	brick->DiffuseMap("Bricks.png");
	brick->Specular(1, 0.3f, 0.3f, 2);
	brick->SpecularMap("Bricks_Specular.png");
	//brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	brick->NormalMap("Bricks_Normal.png");

	//Create Mesh
	{
		Transform* transform = NULL;


		grid = new MeshRender(shader, new MeshGrid(5, 5));
		transform = grid->AddTransform();
		transform->Position(50, -10, 50);
		transform->Scale(1, 1, 1);

		sphere = new MeshRender(shader, new MeshSphere(10.5f));
		transform = sphere->AddTransform();
		transform->Position(0, 100, 0);
		transform->Scale(1, 1, 1);
	}
	grid->UpdateTransforms();
	sphere->UpdateTransforms();
	meshes.push_back(grid);
	meshes.push_back(sphere);

}

void TheWorldLOD::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMaterial(L"B787/Airplane");
	airplane->ReadMesh(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(1000, 9.91f, 1000);
	transform->Scale(0.02, 0.02, 0.02);
	airplane->UpdateTransforms();

	models.push_back(airplane);

}

void TheWorldLOD::KachujinA()
{
	weapon = new Model();
	weapon->ReadMaterial(L"Weapon/Sword");
	weapon->ReadMesh(L"Weapon/Sword");

	kachujinA = new ModelAnimator(shader);
	kachujinA->ReadMaterial(L"kachujin/Mesh");
	kachujinA->ReadMesh(L"kachujin/Mesh");
	kachujinA->ReadClip(L"Kachujin/Idle");
	kachujinA->ReadClip(L"Kachujin/Running");
	kachujinA->ReadClip(L"Kachujin/Jump");
	kachujinA->ReadClip(L"Kachujin/Hip_Hop_Dancing");

	Transform attachTransform;

	attachTransform.Position(-10, 0, -10);
	attachTransform.Scale(1.0f, 1.0f, 1.0f);

	kachujinA->GetModel()->Attach(shader, weapon, 35, &attachTransform);


	Transform* transform = NULL;

	transform = kachujinA->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujinA->PlayClip(0, 0, 1.0f);
	kachujinA->UpdateTransforms();
	animators.push_back(kachujinA);

}

void TheWorldLOD::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);

	//AI_Pass(anim);

}
void TheWorldLOD::Player()
{
	player = new Hero(shader, shader2D);//Create player
	player->GetTransform()->Scale(0.1, 0.1, 0.1);
	player->GetTransform()->Position(125, 0, 125);
	//player->Update();

	vector<UI>ui = player->GetUI();//Create UI
	//uis.push_back(ui[0]);
	uis.assign(ui.begin(), ui.end());//ｳｪﾀﾇ hpｹﾙ
	animators.push_back(player->GetPawn());

	if (zombie != NULL)
		zombie->SetTraget(player->GetTransform());

	if (bowMaster != NULL)
		bowMaster->SetTraget(player->GetTransform());

}
void TheWorldLOD::EZombie(UINT count, float x, float z, float Hp, float atk, float defend)
{
	if(swordspawned == false)
		zombie = new Zombie(shader, shader2D);//Create bowMaster
	
	swordspawned = true;
	Transform* transform = NULL;
	countbowm = 10;
	for (int i = 0; i < count; i++)
	{
		Vector3 random = Math::RandomVec3(0, 256);
		random.y = 0;

		transform = zombie->AddTransform(Hp, atk, defend);
		if(enemyTypes.Random)
			transform->Position(random);
		else
			transform->Position(Vector3(x, 0, z));
		transform->Scale(0.1f, 0.1f, 0.1f);
	}
	zombie->InitTransforms();
	if(player != NULL)
	zombie->SetTraget(player->GetTransform());	//ﾅｸｰﾙﾁ､

	zombie->SetFindRange(100);					//ﾅｽﾁ�ﾀｧ
	zombie->SetAttackRange(10);					//ｰﾝｼﾓｵｵ
	zombie->SetSpeed(100);						//ﾀﾌｵｿｼﾓｵｵ
	zombie->CreateRandomPath(10, 15);				//ｹ霾ｸｹ・ｧｰ・ｹ霾ｸ ﾆﾎﾆｮ

	vector<UI> ui = zombie->GetUI();				//ﾀ釥ﾇ hp
	for (int i = 0; i < ui.size(); i++)
		uis.push_back(ui[i]);

	animators.push_back(zombie->GetPawn());
}
void TheWorldLOD::Archer(UINT count, float x, float z, float Hp, float atk, float defend)
{
	if (archerspawned == false)
		bowMaster = new BowMaster(shader,shader2D);//Create bowMaster
	archerspawned = true;
	Transform* transform = NULL;
	Transform* artransform = NULL;

	countbowm = 10;
	for (int i = 0; i < count; i++)
	{
		Vector3 random = Math::RandomVec3(0, 256);
		random.y = 0;

		transform = bowMaster->AddTransform(Hp,atk,defend);

		if (enemyTypes.Random)
			transform->Position(random);
		else
			transform->Position(Vector3(x, 0, z));

		transform->Scale(0.1f, 0.1f, 0.1f);
	}
	bowMaster->InitTransforms();
	if (player != NULL)
	bowMaster->SetTraget(player->GetTransform());	//ﾅｸｰﾙﾁ､

	bowMaster->SetFindRange(100);					//ﾅｽﾁ・?
	bowMaster->SetAttackRange(50);					//ｰﾝｹ・?
	bowMaster->SetSpeed(100);						//ﾀﾌｵｿｼﾓｵｵ
	bowMaster->CreateRandomPath(10, 15);				//ｹ霾ｸｹ・ｧｰ・ｹ霾ｸ ﾆﾎﾆｮ

	vector<UI> ui = bowMaster->GetUI();				//ﾀ釥ﾇ hp
	for (int i = 0; i < ui.size(); i++)
		uis.push_back(ui[i]);

	animators.push_back(bowMaster->GetPawn());
	models.push_back(bowMaster->GetArrow());
}

void TheWorldLOD::SP()
{

	//Player Update
	{
		//Vector3 position;  player->GetTransform()->Position(&position);
		//float distance1, distance2;

		//Transform* tr = bowMaster->GetNearInstance(position, &distance1);//ﾇﾃｷ｡ﾀﾌｾ鉀ｸｰﾙ

		if (player != NULL)
		player->Update(terrain);
	}
	//bowMaster Update
	{
		if (bowMaster != NULL)
		bowMaster->Update(terrain);
	}
	{
		if (zombie != NULL)
		zombie->Update(terrain);
	}

	//Collider Check
	{
		//Player Attack
		//if(bowMaster->IsDead(true))
		//player->Attack(bowMaster);
		if (player != NULL)
		{
			if (zombie != NULL)
				player->Attack(zombie);
			if (bowMaster != NULL)
				player->Attack(bowMaster);


			//bowMaster Attack
			if (bowMaster != NULL)
				bowMaster->Attack(player);
			if (zombie != NULL)
				zombie->Attack(player);
			if (player->Getdead())
			{
				if (bowMaster != NULL)
					bowMaster->SetTraget(0);
				if (zombie != NULL)
					zombie->SetTraget(0);
			}



	//projector->Update();
	//Vector3 porjpos = player->GetTransform()->GetPosition();
	//if (player->Getspatk())//ｹ・ｧｰ?
	//{
	//	projector->GetCamera()->Position(porjpos.x, 30, porjpos.z);
	//	effectlight->Add(porjpos);
	//	effectlight->Update();
	//	if (bowMaster != NULL)
	//	for (int i = 0; i < bowMaster->GetCount(); i++)
	//	{
	//		Vector3 bowMasterpos = bowMaster->GetHurtCollider(i)->GetTransform()->GetPosition();//ﾀ釥ﾇ ﾃ豬ｹﾃｼﾀｧﾄ｡
	//		float dx = bowMasterpos.x - porjpos.x;
	//		float dz = bowMasterpos.z - porjpos.z;
	//		float dist = sqrt(dx * dx + dz * dz);
	//		if (dist <= 100)//ｹ・ｧｳｻｿ?ﾀﾖﾀｸｸ・ｴ・ﾌﾁ・ｹﾞﾀｽ
	//		{
	//			bowMaster->Damage(i, 20);
	//		}

	//	}
	//	if (zombie != NULL)
	//	for (int i = 0; i < zombie->GetCount(); i++)
	//	{
	//		Vector3 zombiepos = zombie->GetHurtCollider(i)->GetTransform()->GetPosition();//ﾀ釥ﾇ ﾃ豬ｹﾃｼﾀｧﾄ｡
	//		float dx = zombiepos.x - porjpos.x;
	//		float dz = zombiepos.z - porjpos.z;
	//		float dist = sqrt(dx * dx + dz * dz);
	//		if (dist <= 100)//ｹ・ｧｳｻｿ?ﾀﾖﾀｸｸ・ｴ・ﾌﾁ・ｹﾞﾀｽ
	//		{
	//			zombie->Damage(i, 20);
	//		}

	//	}

	//}
	//else
	//	projector->GetCamera()->Position(0, 0, 0);
		}


	}

}

void TheWorldLOD::AI_Pass(UINT val)
{

}

void TheWorldLOD::Play(float x, float z, float Hp, float atk, float defend)
{

	player = new Hero(shader, shader2D);//Create player
	player->GetTransform()->Scale(0.1, 0.1, 0.1);
	player->GetTransform()->Position(x, 0, z);
	//player->Update();

	vector<UI>ui = player->GetUI();//Create UI
	uis.assign(ui.begin(), ui.end());//ｳｪﾀﾇ hpｹﾙ
	animators.push_back(player->GetPawn());

	if (zombie != NULL)
		zombie->SetTraget(player->GetTransform());

	if (bowMaster != NULL)
		bowMaster->SetTraget(player->GetTransform());
	//Player();
}

void TheWorldLOD::Spawn(UINT Type, UINT count, float x, float z, float Hp, float atk, float defend)
{
	if (Type == 1)
	{
		EZombie(count, x, z, Hp, atk, defend);
	}	
	if (Type == 2)
	{
		Archer(count, x, z, Hp, atk, defend);
	}
		
}

void TheWorldLOD::LightSpawn(UINT Type, Color ambient, Color diffuse, Color Specular, Color Emissive, Vector3 Position, float Range, Vector3 Direction, float Angle, float intensity, bool debug)
{
	if (Type == 0)
	{
		AddPointLights();
		AddSpotLights();
	}
	if (Type == 1)
	{
		AddPointLights(ambient, diffuse, Specular, Emissive, Position, Range, intensity, debug);
	}
	if (Type == 2)
	{
		AddSpotLights(ambient, diffuse, Specular, Emissive, Position, Range, Direction, Angle, intensity, debug);
	}
}


void TheWorldLOD::OnGUI()
{
	float windowWidth = 500;

	float width = D3D::Width();
	float height = D3D::Height();

	bool bOpen = true;
	bOpen = ImGui::Begin("Particle", &bOpen);
	ImGui::SetWindowPos(ImVec2(width - windowWidth, 0));
	ImGui::SetWindowSize(ImVec2(windowWidth, height));
	{
		OnGUI_List();
		OnGUI_Setttings();
	}
	ImGui::End();
}

void TheWorldLOD::OnGUI_List()
{
	if (ImGui::CollapsingHeader("Particle List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < particleList.size(); i++)
		{
			if (ImGui::Button(String::ToString(particleList[i]).c_str(), ImVec2(200, 0)))
			{
				SafeDelete(particle);

				file = particleList[i];
				particle = new ParticleSystem(particleList[i]);
				maxParticle = particle->GetData().MaxParticles;
			}
		}//for(i)
	}//ImGui::CollapsingHeader
}

void TheWorldLOD::OnGUI_Write()
{
	ImGui::Separator();

	if (ImGui::Button("WriteParticle"))
	{
		D3DDesc desc = D3D::GetDesc();

		Path::SaveFileDialog
		(
			file, L"Particle file\0*.xml", L"../../_Textures/Particles",
			bind(&TheWorldLOD::WriteFile, this, placeholders::_1),
			desc.Handle
		);
	}
}

void TheWorldLOD::OnGUI_Setttings()
{
	if (particle == NULL) return;

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Separator();

		ImGui::SliderInt("MaxParticles", (int *)&maxParticle, 1, 1000);

		if (ImGui::Button("Apply"))
		{
			particle->GetData().MaxParticles = maxParticle;
			particle->Reset();
		}

		ImGui::Separator();

		const char* types[] = { "Opaque", "Additive", "AlphaBlend" };
		ImGui::Combo("BlendType", (int *)&particle->GetData().Type, types, 3);

		ImGui::SliderFloat("Duration", &particle->GetData().Duration, 0.1f, 10.0f);
		ImGui::SliderFloat("DurationRandomness", &particle->GetData().DurationRandomness, 0.0f, 10.0f);

		ImGui::SliderFloat("EmitterVelocitySensitivity", &particle->GetData().EmitterVelocitySensitivity, 0.0f, 10.0f);

		ImGui::SliderFloat("MinHorizontalVelocity", &particle->GetData().MinHorizontalVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("MaxHorizontalVelocity", &particle->GetData().MaxHorizontalVelocity, -100.0f, 100.0f);

		ImGui::SliderFloat("MinVerticalVelocity", &particle->GetData().MinVerticalVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("MaxVerticalVelocity", &particle->GetData().MaxVerticalVelocity, -100.0f, 100.0f);

		ImGui::SliderFloat3("Gravity", particle->GetData().Gravity, -100, 100);

		ImGui::SliderFloat("EndVelocity", &particle->GetData().EndVelocity, -100.0f, 100.0f);

		ImGui::ColorEdit4("MinColor", particle->GetData().MinColor);
		ImGui::ColorEdit4("MaxColor", particle->GetData().MaxColor);

		ImGui::SliderFloat("MinRotateSpeed", &particle->GetData().MinRotateSpeed, -10, 10);
		ImGui::SliderFloat("MaxRotateSpeed", &particle->GetData().MaxRotateSpeed, -10, 10);

		ImGui::SliderFloat("MinStartSize", &particle->GetData().MinStartSize, 0, 500);
		ImGui::SliderFloat("MaxStartSize", &particle->GetData().MaxStartSize, 0, 500);

		ImGui::SliderFloat("MinEndSize", &particle->GetData().MinEndSize, 0, 500);
		ImGui::SliderFloat("MaxEndSize", &particle->GetData().MaxEndSize, 0, 500);

		ImGui::Spacing();
		OnGUI_Write();


		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (wstring textureFile : textureList)
			{
				if (ImGui::Button(String::ToString(textureFile).c_str(), ImVec2(200, 0)))
					particle->SetTexture(textureFile);
			}//for(i)
		}
	}
}

void TheWorldLOD::UpdateParticleList()
{
	particleList.clear();
	Path::GetFiles(&particleList, L"../../_Textures/Particles/", L"*.xml", false);

	for (wstring& file : particleList)
		file = Path::GetFileNameWithoutExtension(file);
}

void TheWorldLOD::UpdateTextureList()
{
	textureList.clear();

	vector<wstring> files;
	Path::GetFiles(&files, L"../../_Textures/Particles/", L"*.*", false);

	for (wstring file : files)
	{
		wstring ext = Path::GetExtension(file);
		transform(ext.begin(), ext.end(), ext.begin(), toupper);

		file = Path::GetFileName(file);
		if (ext == L"PNG" || ext == L"TGA" || ext == L"JPG")
			textureList.push_back(file);
	}
}

void TheWorldLOD::WriteFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Particle");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);


	Xml::XMLElement* node = NULL;

	node = document->NewElement("BlendState");
	node->SetText((int)particle->GetData().Type);
	root->LinkEndChild(node);


	string textureFile = String::ToString(particle->GetData().TextureFile);
	String::Replace(&textureFile, "Particles/", "");

	node = document->NewElement("TextureFile");
	node->SetText(textureFile.c_str());
	root->LinkEndChild(node);


	node = document->NewElement("MaxParticles");
	node->SetText(particle->GetData().MaxParticles);
	root->LinkEndChild(node);


	node = document->NewElement("BlendSDurationtate");
	node->SetText(particle->GetData().Duration);
	root->LinkEndChild(node);

	node = document->NewElement("DurationRandomness");
	node->SetText(particle->GetData().DurationRandomness);
	root->LinkEndChild(node);

	node = document->NewElement("EmitterVelocitySensitivity");
	node->SetText(particle->GetData().EmitterVelocitySensitivity);
	root->LinkEndChild(node);


	node = document->NewElement("MinHorizontalVelocity");
	node->SetText(particle->GetData().MinHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxHorizontalVelocity");
	node->SetText(particle->GetData().MaxHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MinVerticalVelocity");
	node->SetText(particle->GetData().MinVerticalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxVerticalVelocity");
	node->SetText(particle->GetData().MaxVerticalVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("Gravity");
	node->SetAttribute("X", particle->GetData().Gravity.x);
	node->SetAttribute("Y", particle->GetData().Gravity.y);
	node->SetAttribute("Z", particle->GetData().Gravity.z);
	root->LinkEndChild(node);

	node = document->NewElement("EndVelocity");
	node->SetText(particle->GetData().EndVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("MinColor");
	node->SetAttribute("R", particle->GetData().MinColor.r);
	node->SetAttribute("G", particle->GetData().MinColor.g);
	node->SetAttribute("B", particle->GetData().MinColor.b);
	node->SetAttribute("A", particle->GetData().MinColor.a);
	root->LinkEndChild(node);

	node = document->NewElement("MaxColor");
	node->SetAttribute("R", particle->GetData().MaxColor.r);
	node->SetAttribute("G", particle->GetData().MaxColor.g);
	node->SetAttribute("B", particle->GetData().MaxColor.b);
	node->SetAttribute("A", particle->GetData().MaxColor.a);
	root->LinkEndChild(node);


	node = document->NewElement("MinRotateSpeed");
	node->SetText(particle->GetData().MinRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MaxRotateSpeed");
	node->SetText(particle->GetData().MaxRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MinStartSize");
	node->SetText((int)particle->GetData().MinStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxStartSize");
	node->SetText((int)particle->GetData().MaxStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MinEndSize");
	node->SetText((int)particle->GetData().MinEndSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxEndSize");
	node->SetText((int)particle->GetData().MaxEndSize);
	root->LinkEndChild(node);

	wstring folder = Path::GetDirectoryName(file);
	wstring fileName = Path::GetFileNameWithoutExtension(file);

	document->SaveFile(String::ToString(folder + fileName + L".xml").c_str());
	SafeDelete(document);

	UpdateParticleList();
}


void TheWorldLOD::AddPointLights(Color ambient, Color diffuse, Color Specular, Color Emissive, Vector3 Position, float Range, float intensity, bool debug)
{
	PointLight light;
	light =
	{
		ambient, //Ambient
		diffuse, //Diffuse
		Specular, //Specular
		Emissive, //Emissive
		Position, Range,intensity
	};
	Lights::Get()->AddPointLight(light);

}

void TheWorldLOD::AddPointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.0f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(1200, 15, 1200), 15.0f, 0.9f
	};
	Lights::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.6f, 0.2f, 0.0f, 1.0f),
		Color(0.6f, 0.3f, 0.0f, 1.0f),
		Vector3(1250, 15, 1230), 30.0f, 0.3f
	};
	Lights::Get()->AddPointLight(light);


	for (float z = 1000; z <= 1500; z += 150)
	{
		for (float x = 1000; x <= 1500; x += 150)
		{
			light =
			{
				Color(0, 0, 0, 1),
				Math::RandomColor3(),
				Color(0, 0, 0, 1),
				Color(0, 0, 0, 1),
				Vector3(x, 15, z), 40, Math::Random(0.1f, 1.0f)
			};

			Lights::Get()->AddPointLight(light);
		}
	}
}

void TheWorldLOD::AddSpotLights(Color ambient, Color diffuse, Color Specular, Color Emissive, Vector3 Position, float Range, Vector3 Direction, float Angle, float intensity, bool debug)
{
	SpotLight light;
	light =
	{
		ambient, //Ambient
		diffuse, //Diffuse
		Specular, //Specular
		Emissive, //Emissive
		Position, Range,
		Direction,Angle,intensity
	};
	Lights::Get()->AddSpotLight(light);
}

void TheWorldLOD::AddSpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.7f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Vector3(1000, 50, 1000), 80.0f,
		Vector3(0, -1, 0), 30.0f, 0.9f
	};
	Lights::Get()->AddSpotLight(light);

	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Vector3(1030, 50, 1500), 80.0f,
		Vector3(0, -1, 0), 40.0f, 0.9f
	};
	Lights::Get()->AddSpotLight(light);
}


void TheWorldLOD::SaveMap(wstring file)
{
	terrain->CreateTestTexture(file);
}

void TheWorldLOD::OpenMap(wstring file)
{
	SafeDelete(terrain);

	terrain = new Terrain(shader, file);
}

void TheWorldLOD::OpenLayer(wstring name)
{
	terrain->LayerMap1(name);
}

void TheWorldLOD::OpenLayer1(wstring name)
{
	terrain->LayerMap2(name);
}

void TheWorldLOD::OpenLayer2(wstring name)
{
	terrain->LayerMap3(name);
}

void TheWorldLOD::SetBlur()
{
	float x = 1.0f / D3D::Width();
	float y = 1.0f / D3D::Height();

	GetBlurParameter(weightX, offsetX, x, 0);
	GetBlurParameter(weightY, offsetY, 0, y);

}

void TheWorldLOD::GetBlurParameter(vector<float>& weights, vector<Vector2>& offsets, float x, float y)
{
	weights.clear();
	weights.assign(blurCount, float());

	offsets.clear();
	offsets.assign(blurCount, Vector2());

	weights[0] = GetGaussFunction(0); //1
	offsets[0] = Vector2(0, 0);


	float sum = weights[0];
	for (UINT i = 0; i < blurCount / 2; i++)
	{
		float temp = GetGaussFunction((float)(i + 1));

		weights[i * 2 + 1] = temp;
		weights[i * 2 + 2] = temp;
		sum += temp * 2;

		Vector2 temp2 = Vector2(x, y) * (i * 2 + 1.5f);
		offsets[i * 2 + 1] = temp2;
		offsets[i * 2 + 2] = -temp2;
	}

	for (UINT i = 0; i < blurCount; i++)
		weights[i] /= sum;
}

float TheWorldLOD::GetGaussFunction(float val)
{
	return (float)((1.0 / sqrt(2 * Math::PI * blurCount)) * exp(-(val * val) / (2 * blurCount * blurCount)));
}
