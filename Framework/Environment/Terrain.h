#pragma once
struct Keyframe
{
	Vector3 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 uv;

};

class Terrain : public Renderer
{
public:
	typedef VertexTextureNormalTangent TerrainVertex;

public:
	Terrain(Shader* shader, wstring heightFile);
	~Terrain();

	void Update();
	void Render();
public:
	void BaseMap(wstring file);
	void LayerMap(wstring file, wstring alphaFile);
	void AlphaMap(wstring alpha);
	void LayerMap1(wstring layer);
	void LayerMap2(wstring layer);
	void LayerMap3(wstring layer);

	void NormalMap(wstring file);
	void SpecularMap(wstring file);
	void DetailMap(wstring file);
	void SnowMap(wstring file);
	void DistanceMap(wstring file);

public:
	float GetHeight(Vector3& position);
	float GetHeightPick(Vector3& position);
	Vector3 GetPickedPosition();
	void RaiseHeight(Vector3& position, UINT type, UINT range);

	void CreateTestTexture(wstring file);
	void LayerBrush(Vector3& position, UINT range, UINT type);
	void DrawTextureBrush(Vector3 & position, UINT range, UINT type);

	void SetTransform(Transform* stransform);

public:
	void SaveXmlfile(wstring file);
	void Loadfile(wstring file);
	void OpenTexture(wstring filename);

private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();

	

private:
	struct BrushDesc
	{
		Color Color = D3DXCOLOR(0, 1, 0, 1);
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 1;
		float Padding[3];
	} brushDesc;

	struct LineDesc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);
		UINT Visible = 0;
		float Thickness = 0.01f;
		float Size = 5.0f;
		float Padding;
	} lineDesc;

	Color LayerColor = D3DXCOLOR(0, 0, 0, 1);

private:
	Texture* heightMap;

	vector<Keyframe> keyframes;
	vector<string> layerMaps;
	vector<Color> alpha_pixel_color;

	string baseName;
	string layer1Name;
	string layer2Name;
	string layer3Name;
	UINT layerNum;

	UINT width;
	UINT height;

	
	TerrainVertex* vertices;	
	UINT* indices;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap = NULL;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* layerMap1 = NULL;
	ID3DX11EffectShaderResourceVariable* sLayerMap1;

	Texture* layerMap2 = NULL;
	ID3DX11EffectShaderResourceVariable* sLayerMap2;


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

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* lineBuffer;
	ID3DX11EffectConstantBuffer* sLineBuffer;
	//Transform* transform = NULL;

};