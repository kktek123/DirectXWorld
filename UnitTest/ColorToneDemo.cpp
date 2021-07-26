#include "stdafx.h"
#include "ColorToneDemo.h"

void ColorToneDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0, 0);
	Context::Get()->GetCamera()->Position(0, 32, -67);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);


	shader = new Shader(L"51_SpotLight.fxo");	
	gBuffer = new GBuffer(shader);

	renderTarget = new RenderTarget();
	depthStencil = new DepthStencil();
	viewport = new Viewport(D3D::Width(), D3D::Height());

	postEffect = new PostEffect(L"52_ColorTone.fxo", gBuffer->Srvs());
	postEffect->Srv(0, renderTarget->SRV());

	render2D = new Render2D();
	render2D->GetTransform()->Position(200, 120, 0);
	render2D->GetTransform()->Scale(355.5f, 200, 1);
	render2D->SRV(renderTarget->SRV());

	
	sky = new CubeSky(L"Environment/GrassCube1024.dds");
	snow = new Snow(Vector3(300, 100, 500), (UINT)1e+4, L"Environment/Snow.png");

	Mesh();
	Airplane();
	Kachujin();

	AddPointLights();
	AddSpotLights();
}

void ColorToneDemo::Destroy()
{

}

void ColorToneDemo::Update()
{
	//Perspective
	{
		ImGui::Separator();

		Perspective* perspective = Context::Get()->GetPerspective();
		static float fov = 0.25f;
		ImGui::InputFloat("Fov", &fov, 0.01f);

		static float width = D3D::Width();
		ImGui::InputFloat("Width", &width, 0.1f);

		static float height = D3D::Height();
		ImGui::InputFloat("Height", &height, 0.1f);

		perspective->Set(width, height, 1, 1000, Math::PI * fov);
	}


	static UINT pass = postEffect->GetShader()->PassCount() - 1;
	ImGui::InputInt("Pass", (int *)&pass);
	pass %= shader->PassCount();
	postEffect->Pass(pass);

	//Saturation
	{
		ImGui::Separator();

		static float Saturation = 0.0f;
		ImGui::InputFloat("Saturation", &Saturation, 0.1f);
		postEffect->GetShader()->AsScalar("Saturation")->SetFloat(Saturation);
	}
	
	//Sharpness
	{
		ImGui::Separator();

		static float Sharpness = 0.0f;
		ImGui::InputFloat("Sharpness", &Sharpness, 0.1f);
		postEffect->GetShader()->AsScalar("Sharpness")->SetFloat(Sharpness);
	}
	
	//Vignette
	{
		ImGui::Separator();

		static float Power = 2.0f;
		ImGui::InputFloat("Power", &Power, 0.01f);
		postEffect->GetShader()->AsScalar("Power")->SetFloat(Power);

		static float ScaleX = 2.0f;
		ImGui::InputFloat("ScaleX", &ScaleX, 0.01f);
		
		static float ScaleY = 2.0f;
		ImGui::InputFloat("ScaleY", &ScaleY, 0.01f);
		postEffect->GetShader()->AsVector("Scale")->SetFloatVector(Vector2(ScaleX, ScaleY));
	}

	//Interace
	{
		ImGui::Separator();

		static float Strength = 1.0f;
		ImGui::InputFloat("Strength", &Strength, 0.01f);
		postEffect->GetShader()->AsScalar("Strength")->SetFloat(Strength);

		static int InteraceValue = 2;
		ImGui::InputInt("InteraceValue", &InteraceValue);
		postEffect->GetShader()->AsScalar("interaceValue")->SetInt(InteraceValue);
	}

	//Interace
	{
		ImGui::Separator();

		static float LensPower = 1.0f;
		ImGui::InputFloat("LensPower", &LensPower, 0.01f);
		postEffect->GetShader()->AsScalar("LensPower")->SetFloat(LensPower);

		static float Distortion = -0.02f;
		ImGui::InputFloat("Distortion", &Distortion, 0.001f);
		
		static float ChromaticDistortion = -0.01f;
		ImGui::InputFloat("ChromaticDistortion", &ChromaticDistortion, 0.001f);

		Vector3 temp = Vector3(Distortion, 0, 0);
		temp.y = Distortion + ChromaticDistortion;
		temp.z = Distortion + 2 * ChromaticDistortion;

		postEffect->GetShader()->AsVector("Distortion")->SetFloatVector(temp);
	}

	//Down Scale
	{
		ImGui::Separator();

		static float ScaleX = D3D::Width();
		ImGui::InputFloat("DownScaleX", &ScaleX, 1.0f);

		static float ScaleY = D3D::Height();
		ImGui::InputFloat("DownScaleY", &ScaleY, 1.0f);
		postEffect->GetShader()->AsVector("ScaleSourceSize")->SetFloatVector(Vector2(ScaleX, ScaleY));
	}
	


	sky->Update();

	sphere->Update();
	cylinder->Update();
	cube->Update();
	grid->Update();

	airplane->Update();
	kachujin->Update();

	snow->Update();

	postEffect->Update();
	render2D->Update();
}

void ColorToneDemo::PreRender()
{
	//PackGBuffer
	{
		gBuffer->PreRender();

		Pass(3, 4, 5);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		floor->Render();
		grid->Render();

		airplane->Render();
		kachujin->Render();
	}

	//Main Render
	{
		renderTarget->Set(depthStencil);
		viewport->RSSetViewport();

		sky->Render();

		gBuffer->Render(renderTarget);

		snow->Render();
	}
}

void ColorToneDemo::Render()
{
	postEffect->Render();
	render2D->Render();
}

void ColorToneDemo::PostRender()
{
	
}

void ColorToneDemo::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->Specular(1, 1, 1, 15);
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");

		stone = new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->Specular(1, 1, 1, 2);
		stone->SpecularMap("Stones_Specular.png");
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
		stone->NormalMap("Stones_Normal.png");
		

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->Specular(1, 0.3f, 0.3f, 2);
		brick->SpecularMap("Bricks_Specular.png");
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
		brick->NormalMap("Bricks_Normal.png");
		

		wall = new Material(shader);
		wall->DiffuseMap("Wall.png");
		wall->Specular(1, 1, 1, 2);
		wall->SpecularMap("Wall_Specular.png");
		wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
		wall->NormalMap("Wall_Normal.png");
	}

	//Create Mesh
	{
		Transform* transform = NULL;

		cube = new MeshRender(shader, new MeshCube());
		transform = cube->AddTransform();
		transform->Position(0, 5, 0);
		transform->Scale(20, 10, 20);

		grid = new MeshRender(shader, new MeshGrid(5, 5));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		//transform->Position(0, 5, 0);
		transform->Scale(12, 1, 12);

		cylinder = new MeshRender(shader, new MeshCylinder(0.5f, 3.0f, 20, 20));
		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->Position(-30, 6, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);

			transform = cylinder->AddTransform();
			transform->Position(30, 6, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);


			transform = sphere->AddTransform();
			transform->Position(-30, 15.5f, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);

			transform = sphere->AddTransform();
			transform->Position(30, 15.5f, -15.0f + (float)i * 15.0f);
			transform->Scale(5, 5, 5);
		}
	}

	sphere->UpdateTransforms();
	cylinder->UpdateTransforms();
	cube->UpdateTransforms();
	grid->UpdateTransforms();

	meshes.push_back(sphere);
	meshes.push_back(cylinder);
	meshes.push_back(cube);
	meshes.push_back(grid);
}

void ColorToneDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMaterial(L"B787/Airplane");
	airplane->ReadMesh(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(2.0f, 9.91f, 2.0f);
	transform->Scale(0.004f, 0.004f, 0.004f);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void ColorToneDemo::Kachujin()
{
	weapon = new Model();
	weapon->ReadMaterial(L"Weapon/Sword");
	weapon->ReadMesh(L"Weapon/Sword");

	kachujin = new ModelAnimator(shader);
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Running");
	kachujin->ReadClip(L"Kachujin/Jump");
	kachujin->ReadClip(L"Kachujin/Hip_Hop_Dancing");

	Transform attachTransform;
	attachTransform.Position(10, -3.5f, -10);
	attachTransform.Scale(1.28f, 1.28f, 1.28f);

	kachujin->GetModel()->Attach(shader, weapon, 35, &attachTransform);


	Transform* transform = NULL;

	transform = kachujin->AddTransform();
	transform->Position(-25, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayClip(0, 0, 0.1f);

	transform = kachujin->AddTransform();
	transform->Position(-10, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayClip(1, 1, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(10, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayClip(2, 2, 0.3f);

	transform = kachujin->AddTransform();
	transform->Position(25, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayClip(3, 3, 0.75f);

	kachujin->UpdateTransforms();

	animators.push_back(kachujin);
}

void ColorToneDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}

void ColorToneDemo::AddPointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.0f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(-25, 10, -30), 5.0f, 0.9f
	};
	Lights::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.6f, 0.2f, 0.0f, 1.0f),
		Color(0.6f, 0.3f, 0.0f, 1.0f),
		Vector3(25, 10, -30), 10.0f, 0.3f
	};
	Lights::Get()->AddPointLight(light);


	for (float z = -30; z <= 30; z += 30)
	{
		for (float x = -30; x <= 30; x += 30)
		{
			light =
			{
				Color(0, 0, 0, 1),
				Math::RandomColor3(),
				Color(0, 0, 0, 1),
				Color(0, 0, 0, 1),
				Vector3(x, 1, z), 5, Math::Random(0.1f, 1.0f)
			};

			Lights::Get()->AddPointLight(light);
		}
	}
}

void ColorToneDemo::AddSpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.7f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Vector3(-10, 20, -30), 25.0f,
		Vector3(0, -1, 0), 30.0f, 0.9f
	};
	Lights::Get()->AddSpotLight(light);

	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Vector3(10, 20, -30), 30.0f,
		Vector3(0, -1, 0), 40.0f, 0.9f
	};
	Lights::Get()->AddSpotLight(light);
}
