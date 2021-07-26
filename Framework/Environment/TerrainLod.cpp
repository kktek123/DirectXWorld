#include "Framework.h"
#include "TerrainLod.h"

TerrainLod::TerrainLod(InitializeDesc & desc)
	: Renderer(desc.shader), initDesc(desc)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	sBaseMap = shader->AsSRV("BaseMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sAlphaMap = shader->AsSRV("AlphaMap");
	sHeightMap = shader->AsSRV("HeightMap");
	sNormalMap = shader->AsSRV("NormalMap");
	sSpecularMap = shader->AsSRV("SpecularMap");
	sDetailMap = shader->AsSRV("DetailMap");
	sSnowMap = shader->AsSRV("SnowMap");
	sDistanceMap = shader->AsSRV("DistanceMap");

	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLod");


	heightMap = new Texture(desc.heightMap);
	sHeightMap->SetResource(heightMap->SRV());
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UINT, &heightMapPixel);
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heightMaps);
	width = this->heightMap->GetWidth() - 1;
	height = this->heightMap->GetHeight() - 1;


	vertexPerPatchX = (width / desc.CellsPerPatch) + 1;
	vertexPerPatchZ = (height / desc.CellsPerPatch) + 1;

	vertexCount = vertexPerPatchX * vertexPerPatchZ;
	faceCount = (vertexPerPatchX - 1) * (vertexPerPatchZ - 1);
	indexCount = faceCount * 4;


	CalcBoundY();
	CreateVertexData();
	CreateIndexData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain));
	indexBuffer = new IndexBuffer(indices, indexCount);

	bufferDesc.TexelCellSpaceU = 1.0f / (float)heightMap->GetWidth() - 1;
	bufferDesc.TexelCellSpaceV = 1.0f / (float)heightMap->GetHeight() - 1;
	bufferDesc.HeightRatio = desc.HeightRatio;

	
	camera = new Fixity();
	perspective = new Perspective(D3D::Width(), D3D::Height(), 0.1f, 1000.0f, Math::PI * 0.35f);

	frustum = new Frustum(NULL, perspective);
	//frustum = new Frustum(camera, perspective);
}

TerrainLod::~TerrainLod()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(buffer);
	SafeDelete(frustum);
	SafeDelete(camera);
	SafeDelete(perspective);

	SafeDelete(heightMap);

	SafeDelete(baseMap);
	SafeDelete(layerMap);
	SafeDelete(alphaMap);
	//SafeDelete(specularMap);
	//SafeDelete(detailMap);
	//SafeDelete(snowMap);
}

void TerrainLod::Update()
{
	Super::Update();

	ImGui::SliderFloat("MinDistance", &bufferDesc.MinDistance, 1, 100);
	ImGui::SliderFloat("MaxDistance", &bufferDesc.MaxDistance, 1, 5000);

	camera->Update();

	frustum->Update();
	frustum->Planes(bufferDesc.WorldFrustumPlanes);
}

void TerrainLod::Render()
{
	Super::Render();

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL)
		sLayerMap->SetResource(layerMap->SRV());

	if (alphaMap != NULL)
		sAlphaMap->SetResource(alphaMap->SRV());

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());

	if (specularMap != NULL)
		sSpecularMap->SetResource(specularMap->SRV());


	if (detailMap != NULL)
		sDetailMap->SetResource(detailMap->SRV());

	if (snowMap != NULL)
		sSnowMap->SetResource(snowMap->SRV());

	if (distanceMap != NULL)
		sDistanceMap->SetResource(distanceMap->SRV());

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	shader->DrawIndexed(0, Pass(), indexCount);

}

void TerrainLod::BaseMap(wstring file)
{
	SafeDelete(baseMap);

	baseMap = new Texture(file);
}

void TerrainLod::LayerMap(wstring layer, wstring alpha)
{
	SafeDelete(alphaMap);
	SafeDelete(layerMap);

	alphaMap = new Texture(alpha);
	layerMap = new Texture(layer);
}

void TerrainLod::NormalMap(wstring file)
{
	SafeDelete(normalMap);

	normalMap = new Texture(file);
}

void TerrainLod::SpecularMap(wstring file)
{
	SafeDelete(specularMap);

	specularMap = new Texture(file);
}

void TerrainLod::DetailMap(wstring file)
{
	SafeDelete(detailMap);

	detailMap = new Texture(file);
}

void TerrainLod::SnowMap(wstring file)
{
	SafeDelete(snowMap);

	snowMap = new Texture(file);
}

void TerrainLod::DistanceMap(wstring file)
{
	SafeDelete(distanceMap);

	distanceMap = new Texture(file);
}



Vector3 TerrainLod::GetPosition()
{
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* vp = Context::Get()->GetViewport();

	Vector3 n, f;
	Vector3 mouse = Mouse::Get()->GetPosition();

	mouse.z = 0.0f;
	vp->Unproject(&n, mouse, transform->World(), V, P);

	mouse.z = 1.0f;
	vp->Unproject(&f, mouse, transform->World(), V, P);

	Vector3 start = n;
	Vector3 direction = f - n;

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + x + 1;
			index[3] = width * (z + 1) + x + 1;

			Vector3 p[4];
			for (UINT i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;

			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		}
	}

	return Vector3(-1, FLT_MIN, -1);
}

void TerrainLod::RasieHeight(Vector3 & position, UINT type, UINT range)
{
	//1. 영역을 구함
	D3D11_BOX box;
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;


	//2. 범위 제한
	if (box.left < 0)box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;


	//3. 정점 편집
	for (UINT z = box.bottom; z <= box.top; z++)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			UINT index = width * z + x;

			vertices[index].Position.y += 5.0f * Time::Delta();
		}
	}
	CalcBoundY();


	//4. 정점을 GPU에 복사
	//D3D::GetDC()->UpdateSubresource
	//(
	//	vertexBuffer->Buffer(), 0, NULL, vertices, sizeof(TerrainVertex) * vertexCount, 0
	//);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(VertexTerrain) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

bool TerrainLod::InBounds(UINT x, UINT z)
{
	return x >= 0 && x < width && z >= 0 && z < height;
}

float TerrainLod::GetHeight(Vector3 & position)
{
	float halfWidth = (float)width * 0.5f;
	float halfHeight = (float)height * 0.5f;

	UINT x = (UINT)(position.x + halfWidth);
	UINT z = (UINT)(position.z + halfHeight);

	if (!InBounds(x,z)) return FLT_MIN;

	float patchWidth = (float)width / (float)(vertexPerPatchX - 1);
	float patchHeight = (float)height / (float)(vertexPerPatchZ - 1);
	
	UINT index[4];
	index[0] = patchWidth * z + x;
	index[1] = patchWidth * (z + 1) + x;
	index[2] = patchWidth * z + x + 1;
	index[3] = patchWidth * (z + 1) + x + 1;
	UINT pixel = patchWidth * (patchHeight - 1 - position.z) + position.x;
	UINT patchID = (width - 1) * position.z + position.x;
	Vector3 v[4];
	//v[0] = vertices[index[0]].Position;
	//for (int i = 0; i < 4; i++)
	//	v[i] = vertices[index[i]].Position;

	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	Vector3 result(0, FLT_MIN, 0);
	if (ddx + ddz <= 1)
	{
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	//return result.y;
	return heightMaps[pixel].b * 255 / initDesc.HeightRatio + 5;
}

void TerrainLod::CalcPatchBounds(UINT x, UINT z)
{
	UINT x0 = x * initDesc.CellsPerPatch;
	UINT x1 = (x + 1) * initDesc.CellsPerPatch;

	UINT z0 = z * initDesc.CellsPerPatch;
	UINT z1 = (z + 1) * initDesc.CellsPerPatch;


	float minY = FLT_MAX;
	float maxY = FLT_MIN;

	for (UINT z = z0; z <= z1; z++)
	{
		for (UINT x = x0; x <= x1; x++)
		{
			float y = 0.0f;
			UINT pixel = width * (height - 1 - z) + x;

			if (InBounds(x, z))
				y = heightMapPixel[pixel].b * 255 / initDesc.HeightRatio;

			minY = min(minY, y);
			maxY = max(maxY, y);
		}
	}

	UINT patchID = (vertexPerPatchX - 1) * z + x;
	bounds[patchID] = Vector2(minY, maxY);
}


float TerrainLod::GetHeightPicked(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (InBounds(x, z)) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	float u, v, distance;

	Vector3 start(position.x, 1000, position.z);
	Vector3 direction(0, -1, 0);

	Vector3 result(-1, FLT_MIN, -1);
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;
}



void TerrainLod::CalcBoundY()
{
	bounds.assign(faceCount, Vector2());

	for (UINT z = 0; z < vertexPerPatchZ - 1; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX - 1; x++)
		{
			CalcPatchBounds(x, z);
		}
	}
}

void TerrainLod::CreateVertexData()
{
	vertices = new VertexTerrain[vertexCount];

	float halfWidth = (float)width * 0.5f;
	float halfHeight = (float)height * 0.5f;

	float patchWidth = (float)width / (float)(vertexPerPatchX - 1);
	float patchHeight = (float)height / (float)(vertexPerPatchZ - 1);

	float u = 1.0f / (float)(vertexPerPatchX - 1);
	float v = 1.0f / (float)(vertexPerPatchZ - 1);

	for (UINT z = 0; z < vertexPerPatchZ; z++)
	{
		//float z1 = halfHeight - (float)z * patchHeight;

		for (UINT x = 0; x < vertexPerPatchX; x++)
		{
			//float x1 = -halfWidth + (float)x * patchWidth;
			UINT patchId = vertexPerPatchX * z + x;
			UINT pixel = vertexPerPatchX * (vertexPerPatchZ - 1 - z) + x;

			vertices[patchId].Position = Vector3(x, 0, z);
			vertices[patchId].Uv = Vector2(x * u, z * v);
		}
	}

	for (UINT z = 0; z < vertexPerPatchZ - 1; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX - 1; x++)
		{
			UINT patchID = (vertexPerPatchX - 1) * z + x;
			UINT vertID = vertexPerPatchX * z + x;

			vertices[vertID].BoundsY = bounds[patchID];
		}
	}
}

void TerrainLod::CreateIndexData()
{
	UINT index = 0;
	this->indices = new UINT[indexCount];
	for (UINT z = 0; z < vertexPerPatchZ - 1; z++)
	{
		for (UINT x = 0; x < vertexPerPatchX - 1; x++)
		{
			this->indices[index++] = vertexPerPatchX * z + x;
			this->indices[index++] = vertexPerPatchX * z + x + 1;
			this->indices[index++] = vertexPerPatchX * (z + 1) + x;
			this->indices[index++] = vertexPerPatchX * (z + 1) + x + 1;
		}
	}
}