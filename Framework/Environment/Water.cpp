#include "Framework.h"
#include "Water.h"

Water::Water(Shader * shader, float radius, UINT width, UINT height, UINT x, UINT y ,UINT z)
	: Renderer(shader)
	, radius(radius), width(width), height(height)
{

	GetTransform()->Position(x, y, z);
	vertexCount = 4;
	indexCount = 6;

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-radius, 0.0f, -radius);
	vertices[1].Position = Vector3(-radius, 0.0f, +radius);
	vertices[2].Position = Vector3(+radius, 0.0f, -radius);
	vertices[3].Position = Vector3(+radius, 0.0f, +radius);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = new IndexBuffer(indices, indexCount);

	sClipPlane = shader->AsVector("ClipPlane");

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Water");


	this->width = width > 0 ? width : (UINT)D3D::Width();
	this->height = height > 0 ? height : (UINT)D3D::Height();

	reflection = new Reflection(shader, transform, this->width, this->height);
	refraction = new Refraction(shader, L"Environment/Wave.dds", this->width, this->height);

}

Water::~Water()
{
	SafeDelete(buffer);

	SafeDelete(reflection);
	SafeDelete(refraction);
}

void Water::Update()
{
	Super::Update();

	desc.WaveTranslation += desc.WaveSpeed * Time::Delta();
	if (desc.WaveTranslation > 10.0f)
		desc.WaveTranslation -= 10.0f;

	reflection->Update();
	refraction->Update();

	//static Plane plane(1, 0, 0, 0);
	//ImGui::SliderFloat3("Plane", plane, -1, 1);
	//ImGui::SliderFloat("Distance", &plane.d, 0, 30);

	//sClipPlane->SetFloatVector(plane);
}

void Water::PreRender_Reflection()
{
	Vector3 position;
	transform->Position(&position);

	Plane plane = Plane(0, 1, 0, -position.y);
	sClipPlane->SetFloatVector(plane);


	reflection->PreRender();
}

void Water::PreRender_Refraction()
{
	Vector3 position;
	transform->Position(&position);

	Plane plane = Plane(0, -1, 0, position.y + 1.5f);
	sClipPlane->SetFloatVector(plane);


	refraction->PreRender();
}
void Water::ResetClipPlane()
{
	Plane plane(0, 0, 0, 0);
	sClipPlane->SetFloatVector(plane);
}

void Water::Render()
{
	Super::Render();

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	reflection->Render();
	refraction->Render();

	shader->DrawIndexed(0, Pass(), indexCount);
}
