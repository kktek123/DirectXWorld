#include "Framework.h"
#include "Terrain.h"
#include "Utilities\Xml.h"

Terrain::Terrain(Shader * shader, wstring heightFile)
	: Renderer(shader)
{
	heightMap = new Texture(heightFile);

	sBaseMap = shader->AsSRV("BaseMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sAlphaMap = shader->AsSRV("AlphaMap");
	sNormalMap = shader->AsSRV("NormalMap");
	sSpecularMap = shader->AsSRV("SpecularMap");
	sDetailMap = shader->AsSRV("DetailMap");
	sSnowMap = shader->AsSRV("SnowMap");
	sDistanceMap = shader->AsSRV("DistanceMap");
	sLayerMap1 = shader->AsSRV("LayerMap1");
	sLayerMap2 = shader->AsSRV("LayerMap2");


	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_Brush");

	lineBuffer = new ConstantBuffer(&lineDesc, sizeof(LineDesc));
	sLineBuffer = shader->AsConstantBuffer("CB_TerrainLine");


	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(TerrainVertex), 0, true);
	indexBuffer = new IndexBuffer(indices, indexCount);

	

	//heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);
	//heights = shader->AsVector("LayerColor");
}

Terrain::~Terrain()
{
	SafeDelete(brushBuffer);
	SafeDelete(lineBuffer);
	SafeDelete(heightMap);
	
	SafeDelete(baseMap);
	SafeDelete(layerMap);
	SafeDelete(alphaMap);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}

void Terrain::Update()
{
	Super::Update();
	{
		static bool Splatting = false;
		ImGui::Checkbox("Splatting", &Splatting);

		ImGui::SliderFloat3("Direction", Context::Get()->Direction(), -1, 1);

		ImGui::InputInt("BrushType", (int *)&brushDesc.Type);
		brushDesc.Type %= 4;


		ImGui::InputInt("Range", (int *)&brushDesc.Range);
		//brushDesc.Range %= 20;

		//if (brushDesc.Type > 0)
		//{
		//	brushDesc.Location = GetPickedPosition();

		//	if (Mouse::Get()->Press(0))
		//		RaiseHeight(brushDesc.Location, brushDesc.Type, brushDesc.Range);
		//}


		ImGui::Separator();
		ImGui::InputFloat("Color", lineDesc.Color);

		ImGui::InputInt("Visible", (int *)&lineDesc.Visible);
		lineDesc.Visible %= 2;

		ImGui::InputFloat("Thickness", &lineDesc.Thickness, 0.001f);
		lineDesc.Thickness = Math::Clamp(lineDesc.Thickness, 0.01f, 0.9f);

		ImGui::InputFloat("Size", &lineDesc.Size, 1.0f);

		if (ImGui::Button("All Save"))
		{

			HWND hWnd = NULL;
			function<void(wstring)> f = bind(&Terrain::SaveXmlfile, this, placeholders::_1);
			Path::SaveFileDialog(L"", Path::XmlFilter, L"", f, hWnd);
		}

		if (ImGui::Button("All Load"))
		{
			HWND hWnd = NULL;
			function<void(wstring)> f = bind(&Terrain::Loadfile, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::XmlFilter, L"", f, hWnd);
		}
		if (ImGui::Button("Load"))
		{
			HWND hWnd = NULL;
			function<void(wstring)> f = bind(&Terrain::OpenTexture, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"", f, hWnd);
		}
	
	

	if (brushDesc.Type > 0)
	{
		Vector3 position;
		if (!Mouse::Get()->Press(0) && Keyboard::Get()->Press(VK_SPACE))
		{
			position = GetPickedPosition();
			brushDesc.Location = position;
		}
		if (Mouse::Get()->Press(0) && Keyboard::Get()->Press(VK_SPACE) && !Splatting)
		{
			position = brushDesc.Location;
			RaiseHeight(position, brushDesc.Type,brushDesc.Range);

		}
		if (Mouse::Get()->Press(0) && Keyboard::Get()->Press(VK_SPACE) && Splatting)
		{

			position = GetPickedPosition();
			brushDesc.Location = position;
			//position = brushDesc.Location;
			LayerBrush(position, brushDesc.Range, brushDesc.Type);
			//DrawTextureBrush(position, brushDesc.Range, brushDesc.Type);

		}
		//if (Keyboard::Get()->Down('Q'))
		//{
		//	AlphaMap(L"Terrain/Test.bmp");
		//}
	}

	}
}

void Terrain::Render()
{
	Super::Render();

	static bool bWire = false;
	ImGui::Checkbox("Wireframe", &bWire);
	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL)
	{
		sLayerMap->SetResource(layerMap->SRV());
	}

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


	brushBuffer->Apply();
	sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());

	lineBuffer->Apply();
	sLineBuffer->SetConstantBuffer(lineBuffer->Buffer());

	shader->DrawIndexed(0, Pass(), indexCount);
}

void Terrain::BaseMap(wstring file)
{
	SafeDelete(baseMap);

	baseMap = new Texture(file);
}

void Terrain::LayerMap(wstring file, wstring alphaFile)
{
	SafeDelete(layerMap);
	SafeDelete(alphaMap);

	layerMap = new Texture(file);
	alphaMap = new Texture(alphaFile);
}

void Terrain::LayerMap1(wstring layer)
{
	SafeDelete(layerMap);
	layerMap = new Texture(layer);
	sLayerMap->SetResource(layerMap->SRV());


}
void Terrain::LayerMap2(wstring layer)
{
	SafeDelete(layerMap1);
	layerMap1 = new Texture(layer);

	sLayerMap1->SetResource(layerMap1->SRV());
}

void Terrain::LayerMap3(wstring layer)
{
	SafeDelete(layerMap2);
	layerMap2 = new Texture(layer);

	sLayerMap2->SetResource(layerMap2->SRV());
}

void Terrain::AlphaMap(wstring alpha)
{
	SafeDelete(alphaMap);

	alphaMap = new Texture(alpha);

	sAlphaMap->SetResource(alphaMap->SRV());
}


void Terrain::NormalMap(wstring file)
{
	SafeDelete(normalMap);

	normalMap = new Texture(file);
}

void Terrain::SpecularMap(wstring file)
{
	SafeDelete(specularMap);

	specularMap = new Texture(file);
}

void Terrain::DetailMap(wstring file)
{
	SafeDelete(detailMap);

	detailMap = new Texture(file);
}

void Terrain::SnowMap(wstring file)
{
	SafeDelete(snowMap);

	snowMap = new Texture(file);
}

void Terrain::DistanceMap(wstring file)
{
	SafeDelete(distanceMap);

	distanceMap = new Texture(file);
}

float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x / transform->GetScale().x;
	UINT z = (UINT)position.z / transform->GetScale().z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;

	
	float ddx = (x - v[0].x) / 1.0f;
	float ddz = (z - v[0].z) / 1.0f;

	Vector3 result;

	if (ddx + ddz <= 1.0f)
	{
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1.0f - ddx;
		ddz = 1.0f - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return result.y*transform->GetScale().y;
}

float Terrain::GetHeightPick(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return FLT_MIN;
	if (z < 0 || z > height) return FLT_MIN;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	Vector3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;


	Vector3 start(position.x, 1000, position.z);
	Vector3 direction(0, -1, 0);

	float u, v, distance;
	Vector3 result(-1, FLT_MIN, -1);

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;
}

Vector3 Terrain::GetPickedPosition()
{

	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();

	
	Vector3 n, f;
	mouse.z = 0.0f;
	Vp->Unproject(&n, mouse, transform->World(), V, P);

	mouse.z = 1.0f;
	Vp->Unproject(&f, mouse, transform->World(), V, P);

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
			for (int i = 0; i < 4; i++)
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

void Terrain::RaiseHeight(Vector3 & position, UINT type, UINT range)
{
	D3D11_RECT rect;
	Vector3 p = position;
	
	rect.left = p.x - range;
	rect.top = p.z + range;
	rect.right = p.x + range;
	rect.bottom = p.z - range;


	if (rect.left < 0) rect.left = 0;
	if (rect.top >= height) rect.top = height;
	if (rect.right >= width) rect.right = width;
	if (rect.bottom < 0) rect.bottom = 0;
	if (alpha_pixel_color.size() <= 0)
		D3D11_TEXTURE2D_DESC read_pixel = alphaMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &alpha_pixel_color);

	//정점 편집
	for (UINT z = rect.bottom; z <= rect.top; z++)
	{
		for (UINT x = rect.left; x <= rect.right; x++)
		{
			UINT index = width * z + x;
			float speed;
			float ax = (x - position.x);
			if (ax < 0)ax = -ax;
			float az = (z - position.z);
			if (az < 0)az = -az;
			speed = 5.0f / (2 * ax + 2 * az + 1.0f);
			if (type == 1)
			{
				vertices[index].Position.y += 5.0f*Time::Delta();
			}
			else if (type == 2)
			{
				float dx = vertices[index].Position.x - position.x;
				float dz = vertices[index].Position.z - position.z;
				float dist = sqrt(dx * dx + dz * dz);
				if (dist <= range)
				{
					vertices[index].Position.y += 5.0f * Time::Delta();
				}
			}
			else if (type == 3)
			{
				float dx = vertices[index].Position.x - position.x;
				float dz = vertices[index].Position.z - position.z;
				float dist = sqrt(dx * dx + dz * dz);
				//for (float i = 0; i < range-2.0f; i++)
				//{
				//	if (dist <= range-i)
				//	{
				//		vertices[index].Position.y += (0.5f+i/(range+i)) * Time::Delta();
				//	}
				//}
				if (dist <= range)
				{
					float angle = acosf(dist / range);
					float factor = sinf(angle);
					//(dist * radian > range ? 1.0f : dist * radian / range);
					vertices[index].Position.y += 5.0f*Time::Delta()*(factor);
				}
			}
		}
	}
	CreateNormalData();


	//D3D::GetDC()->UpdateSubresource
	//(
	//	vertexBuffer->Buffer(), 0, NULL, vertices, sizeof(TerrainVertex) * vertexCount, 0
	//);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(TerrainVertex) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

void Terrain::CreateTestTexture(wstring file)
{
	ID3D11Texture2D* texture;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Width = 256;
	desc.Height = 256;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	UINT * pixels = new UINT[256 * 256];
	ZeroMemory(pixels, sizeof(UINT) * 256 * 256);
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;
			pixels[pixel] = D3DXCOLOR(0, 0, vertices[index].Position.y * 10 / 255, 1);
		}
	}
	data.pSysMem = pixels;
	data.SysMemPitch = width * 4;
	data.SysMemSlicePitch = width * height * 4;
	Check(D3D::GetDevice()->CreateTexture2D(&desc, &data, &texture));
	Texture::SaveFile(file, texture);
}

void Terrain::LayerBrush(Vector3 & position, UINT range, UINT type)
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
			UINT pixel = width * (height - 1 - z) + x;
			float dx = vertices[index].Position.x - position.x;
			float dz = vertices[index].Position.z - position.z;
			float dist = sqrt(dx * dx + dz * dz);
			if (dist <= range)
			{
				//if (type == 1)
				//{

				//	heights[pixel].r = 0.5f;
				//}
				//else if (type == 2)
				//{

				//	heights[pixel].r = 0.5f;
				//}
				//else if (type == 3) {

				//	heights[pixel].r = 0.5f;
				//}
			}
		}
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(TerrainVertex)*vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

void Terrain::DrawTextureBrush(Vector3 & position, UINT range, UINT type)
{
}

void Terrain::SetTransform(Transform* stransform)
{
	transform = stransform;
}

void Terrain::SaveXmlfile(wstring file)
{
	Keyframe* key = new Keyframe[width*height];
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;
			key[index].position = vertices[index].Position;
			//key[index].color = heights[pixel];
			//key[index].color = vertices[index].Color;
			key[index].normal = vertices[index].Normal;
			key[index].tangent = vertices[index].Tangent;
			key[index].uv = vertices[index].Uv;

			keyframes.emplace_back(key[index]);
		}
		//vertices가 가진 Position,Color,Normal,Uv 저장
	}

	layerMaps.emplace_back(baseName);
	layerMaps.emplace_back(layer1Name);
	layerMaps.emplace_back(layer2Name);
	layerMaps.emplace_back(layer3Name);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLDeclaration*decl = document->NewDeclaration();
	document->LinkEndChild(decl);
	Xml::XMLElement* root = document->NewElement("Vertices");
	document->LinkEndChild(root);

	for (auto keyframe : keyframes)
	{
		Xml::XMLElement* element = document->NewElement("Vertices");
		root->LinkEndChild(element);


		element->SetAttribute("Position.X", keyframe.position.x);
		element->SetAttribute("Position.Y", keyframe.position.y);
		element->SetAttribute("Position.Z", keyframe.position.z);


		//element->SetAttribute("Color.R", keyframe.color.r);
		//element->SetAttribute("Color.G", keyframe.color.g);
		//element->SetAttribute("Color.B", keyframe.color.b);
		//element->SetAttribute("Color.A", keyframe.color.a);


		element->SetAttribute("Normal.X", keyframe.normal.x);
		element->SetAttribute("Normal.Y", keyframe.normal.y);
		element->SetAttribute("Normal.Z", keyframe.normal.z);

		element->SetAttribute("Tangent.X", keyframe.tangent.x);
		element->SetAttribute("Tangent.Y", keyframe.tangent.y);
		element->SetAttribute("Tangent.Z", keyframe.tangent.z);


		element->SetAttribute("Uv.X", keyframe.uv.x);
		element->SetAttribute("Uv.Y", keyframe.uv.y);


		SafeDelete(key);
	}

	Xml::XMLElement* element = document->NewElement("LayerMaps");
	root->LinkEndChild(element);

	element->SetAttribute("BaseMap", layerMaps[0].c_str());
	element->SetAttribute("LayerMap", layerMaps[1].c_str());
	element->SetAttribute("LayerMap1", layerMaps[2].c_str());
	element->SetAttribute("LayerMap2", layerMaps[3].c_str());


	auto sname = String::ToString(file);
	auto ext = ".Xml";
	document->SaveFile((sname + ext).c_str());
}

void Terrain::Loadfile(wstring file)
{
	SafeDelete(baseMap);
	SafeDelete(layerMap);
	SafeDelete(layerMap1);
	SafeDelete(layerMap2);

	if (!keyframes.empty())
	{
		keyframes.clear();
		keyframes.shrink_to_fit();
	}
	if (!layerMaps.empty())
	{
		layerMaps.clear();
		layerMaps.shrink_to_fit();
	}

	Xml::XMLDocument doc;
	Xml::XMLError error = doc.LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XMLError::XML_SUCCESS);

	Xml::XMLElement* root = doc.FirstChildElement();


	Xml::XMLElement* firstElem = root->FirstChildElement();


	for (; firstElem != nullptr; firstElem = firstElem->NextSiblingElement())
	{
		Keyframe keyframe;

		keyframe.position.x = firstElem->FloatAttribute("Position.X");
		keyframe.position.y = firstElem->FloatAttribute("Position.Y");
		keyframe.position.z = firstElem->FloatAttribute("Position.Z");

		//keyframe.color.r = firstElem->FloatAttribute("Color.R");
		//keyframe.color.g = firstElem->FloatAttribute("Color.G");
		//keyframe.color.b = firstElem->FloatAttribute("Color.B");
		//keyframe.color.a = firstElem->FloatAttribute("Color.A");

		keyframe.normal.x = firstElem->FloatAttribute("Normal.X");
		keyframe.normal.y = firstElem->FloatAttribute("Normal.Y");
		keyframe.normal.z = firstElem->FloatAttribute("Normal.Z");

		keyframe.tangent.x = firstElem->FloatAttribute("Tangent.X");
		keyframe.tangent.y = firstElem->FloatAttribute("Tangent.Y");
		keyframe.tangent.z = firstElem->FloatAttribute("Tangent.Z");

		keyframe.uv.x = firstElem->FloatAttribute("Uv.X");
		keyframe.uv.y = firstElem->FloatAttribute("Uv.Y");

		keyframes.push_back(keyframe);
	}
	Xml::XMLElement* lastElem = root->LastChildElement();
	for (; lastElem != nullptr; lastElem = lastElem->NextSiblingElement())
	{
		baseName = lastElem->Attribute("BaseMap");
		layer1Name = lastElem->Attribute("LayerMap");
		layer2Name = lastElem->Attribute("LayerMap1");
		layer3Name = lastElem->Attribute("LayerMap2");
	}

	for (UINT z = 0; z < height; z++)
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;

			vertices[index].Position.x = keyframes[index].position.x;
			vertices[index].Position.y = keyframes[index].position.y;
			vertices[index].Position.z = keyframes[index].position.z;
			//heights[pixel].r = keyframes[index].color.r;
			//heights[pixel].g = keyframes[index].color.g;
			//heights[pixel].b = keyframes[index].color.b;
			//heights[pixel].a = keyframes[index].color.a;
			vertices[index].Normal.x = keyframes[index].normal.x;
			vertices[index].Normal.y = keyframes[index].normal.y;
			vertices[index].Normal.z = keyframes[index].normal.z;

			vertices[index].Tangent.x = keyframes[index].tangent.x;
			vertices[index].Tangent.y = keyframes[index].tangent.y;
			vertices[index].Tangent.z = keyframes[index].tangent.z;

			vertices[index].Uv.x = keyframes[index].uv.x;
			vertices[index].Uv.y = keyframes[index].uv.y;

		}
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(TerrainVertex)*vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);

	auto path = "Terrain/";
	if (!baseName.empty())
		BaseMap(String::ToWString(path + baseName));
	if (!layer1Name.empty())
		LayerMap1(String::ToWString(path + layer1Name));
	if (!layer2Name.empty())
		LayerMap2(String::ToWString(path + layer2Name));
	if (!layer3Name.empty())
		LayerMap3(String::ToWString(path + layer3Name));
}

void Terrain::OpenTexture(wstring filename)
{
	SafeDelete(alphaMap);

	alphaMap = new Texture(filename);
	sAlphaMap->SetResource(alphaMap->SRV());
}

void Terrain::CreateVertexData()
{
	vector<Color> heights;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();


	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount];
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = (heights[pixel].r * 255.0f) / 3.0f;
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / (float)width;
			vertices[index].Uv.y = (float)(height - 1 - z) / (float)height;
			//vertices[index].Color = Color(1.f, 1.f, 1.f, 1.f);
		}
	}
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * z + x;
			indices[index + 1] = width * (z + 1) + x;
			indices[index + 2] = width * z + x + 1;
			indices[index + 3] = width * z + x + 1;
			indices[index + 4] = width * (z + 1) + x;
			indices[index + 5] = width * (z + 1) + x + 1;

			index += 6;
		}
	}
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		TerrainVertex v0 = vertices[index0];
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];

		//NormalVector
		{
			Vector3 d1 = v1.Position - v0.Position;
			Vector3 d2 = v2.Position - v0.Position;

			Vector3 normal;
			D3DXVec3Cross(&normal, &d1, &d2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}
		
		//TangentVector
		{
			Vector3 d1 = v1.Position - v0.Position;
			Vector3 d2 = v2.Position - v0.Position;

			Vector2 u = v1.Uv - v0.Uv;
			Vector2 v = v2.Uv - v0.Uv;

			float d = 1.0f / (u.x * v.y - u.y * v.x);

			Vector3 tangent;
			tangent.x = (v.y * d1.x - v.x * d2.x) * d;
			tangent.y = (v.y * d1.y - v.x * d2.y) * d;
			tangent.z = (v.y * d1.z - v.x * d2.z) * d;

			vertices[index0].Tangent += tangent;
			vertices[index1].Tangent += tangent;
			vertices[index2].Tangent += tangent;
		}
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
		D3DXVec3Normalize(&vertices[i].Tangent, &vertices[i].Tangent);
	}
		
}