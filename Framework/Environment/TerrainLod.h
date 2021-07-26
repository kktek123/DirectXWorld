#pragma once

class TerrainLod : public Renderer
{
public:
	struct InitializeDesc;
	
public:
	TerrainLod(InitializeDesc& desc);
	~TerrainLod();

	void Update();
	void Render();

	void BaseMap(wstring file);
	void LayerMap(wstring layer, wstring alpha);
	void NormalMap(wstring file);
	void SpecularMap(wstring file);

	void DetailMap(wstring file);
	void SnowMap(wstring file);
	void DistanceMap(wstring file);
public:
	float GetHeight(Vector3& position);
	float GetHeightPicked(Vector3& position);
	Vector3 GetPosition();

	void RasieHeight(Vector3& position, UINT type, UINT range);

private:
	bool InBounds(UINT x, UINT z);
	void CalcPatchBounds(UINT x, UINT z);
	void CalcBoundY();

	void CreateVertexData();
	void CreateIndexData();

public:
	struct InitializeDesc
	{
		Shader* shader;

		wstring heightMap;
		float CellSpacing;
		UINT CellsPerPatch;
		float HeightRatio;
	};

private:
	struct BufferDesc
	{
		float MinDistance = 1.0f;
		float MaxDistance = 5000.0f;
		float MinTessellation = 1.0f;
		float MaxTessellation = 64.0f;

		float TexelCellSpaceU;
		float TexelCellSpaceV;
		float WorldCellSpace = 1.0f;
		float HeightRatio = 10.0f;

		Plane WorldFrustumPlanes[6];
	} bufferDesc;

private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 BoundsY;
	};

private:
	UINT faceCount;

	UINT vertexPerPatchX;
	UINT vertexPerPatchZ;

private:
	InitializeDesc initDesc;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	UINT width, height;

	VertexTerrain* vertices;
	UINT* indices;

	Frustum* frustum;
	Camera* camera;
	Perspective* perspective;

	Texture* heightMap;
	vector<D3DXCOLOR> heightMapPixel;
	vector<Color> heightMaps;
	ID3DX11EffectShaderResourceVariable* sHeightMap;


	vector<Vector2> bounds;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap = NULL;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* alphaMap = NULL;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;

	Texture* normalMap = NULL;
	ID3DX11EffectShaderResourceVariable* sNormalMap;

	Texture* specularMap = NULL;
	ID3DX11EffectShaderResourceVariable* sSpecularMap;

	Texture* detailMap = NULL;
	ID3DX11EffectShaderResourceVariable* sDetailMap;

	Texture* snowMap = NULL;
	ID3DX11EffectShaderResourceVariable* sSnowMap;

	Texture* distanceMap = NULL;
	ID3DX11EffectShaderResourceVariable* sDistanceMap;
};