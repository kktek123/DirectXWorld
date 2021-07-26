#pragma once

class Water : public Renderer
{
public:
	Water(Shader* shader, float radius, UINT width = 0, UINT height = 0, UINT x = 0, UINT y = 0 ,UINT z = 0);
	~Water();

	void Update();

	void ResetClipPlane();
	void PreRender_Reflection();
	void PreRender_Refraction();
	void Render();

private:
	struct Desc
	{
		Color RefractionColor = Color(0.2f, 0.3f, 1.0f, 1.0f);

		Vector2 NormalMapTile = Vector2(0.01f, 0.02f);
		float WaveTranslation = 0.0f;
		float WaveScale = 0.05f;

		float Shininess = 30.0f;
		float Alpha = 1.5f;
		float WaveSpeed = 0.06f;

		float Padding;
	} desc;

private:
	float radius;
	UINT width, height;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Reflection* reflection;
	Refraction* refraction;

	ID3DX11EffectVectorVariable* sClipPlane;
};