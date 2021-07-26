#include "stdafx.h"
#include "ComputeDemo.h"

void ComputeDemo::Initialize()
{
	//Raw();
	//Intersection();

	Texture2D();
}

void ComputeDemo::Destroy()
{
		
}

void ComputeDemo::Update()
{
	if (render2D != NULL) render2D->Update();
}

void ComputeDemo::Render()
{
	if (render2D != NULL) render2D->Render();
}

void ComputeDemo::Raw()
{
	Shader* shader = new Shader(L"29_ByteAddress.fx");

	struct Output
	{
		UINT GroupID[3];
		UINT GroupThreadID[3];
		UINT DispatchThreadID[3];
		UINT GroupIndex;
	};

	UINT size = 2 * 10 * 8 * 3;

	Output* output = new Output[size];
	RawBuffer* buffer = new RawBuffer(NULL, sizeof(Output) * size);

	shader->AsUAV("Output")->SetUnorderedAccessView(buffer->UAV());
	shader->Dispatch(0, 0, 2, 1, 1);


	buffer->Copy(output, sizeof(Output) * size);

	FILE* file;
	fopen_s(&file, "../Cs.csv", "w");

	for (UINT i = 0; i < size; i++)
	{
		Output temp = output[i];

		fprintf
		(
			file,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			i,
			temp.GroupID[0], temp.GroupID[1], temp.GroupID[2],
			temp.GroupThreadID[0], temp.GroupThreadID[1], temp.GroupThreadID[2],
			temp.DispatchThreadID[0], temp.DispatchThreadID[1], temp.DispatchThreadID[2],
			temp.GroupIndex
		);
	}
	fclose(file);
}

void ComputeDemo::Intersection()
{
	Shader* shader = new Shader(L"29_Intersection.fx");

	struct InputDesc
	{
		UINT Index;
		Vector3 V0, V1, V2;
	};

	UINT width = 16;
	UINT height = 16;


	UINT index = 0;
	Vector3* vertices = new Vector3[width * height];
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			index = z * width + x;

			vertices[index] = Vector3((float)x, 0.0f, (float)z);
		}
	}

	index = 0;
	UINT indexCount = (width - 1) * (height - 1) * 6;
	UINT* indices = new UINT[indexCount];
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

	InputDesc* input = new InputDesc[indexCount / 3];
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		input[i].V0 = vertices[index0];
		input[i].V1 = vertices[index1];
		input[i].V2 = vertices[index2];

		input[i].Index = i;
	}

	///////////////////////////////////////////////////////////////////////////

	struct OutputDesc
	{
		UINT Picked;
		float U;
		float V;
		float Distance;
	};


	UINT size = 1 * (indexCount / 3);
	StructuredBuffer* buffer = new StructuredBuffer
	(
		input,
		sizeof(InputDesc), size,
		sizeof(OutputDesc), size
	);

	struct Ray
	{
		Vector3 Position;
		float Padding;

		Vector3 Direction;
		float Padding2;
	} ray;

	ray.Position = Vector3(2.6f, 0.0f, 0.6f);
	ray.Direction = Vector3(0, -1, 0);

	shader->AsVector("Position")->SetFloatVector(ray.Position);
	shader->AsVector("Direction")->SetFloatVector(ray.Direction);
	shader->AsSRV("Input")->SetResource(buffer->SRV());
	shader->AsUAV("Output")->SetUnorderedAccessView(buffer->UAV());

	shader->Dispatch(0, 0, 1, 1, 1);


	OutputDesc* output = new OutputDesc[size];
	buffer->Copy(output, sizeof(OutputDesc) * size);

	FILE* file;
	fopen_s(&file, "../Picked.csv", "w");
	for (UINT i = 0; i < size; i++)
	{
		OutputDesc temp = output[i];

		fprintf(file,
			"%d,%.0f,%.0f,%.0f\n",
			temp.Picked,
			temp.U, temp.V, temp.Distance
		);
	}
	fclose(file);
}

void ComputeDemo::Texture2D()
{
	render2D = new Render2D();
	render2D->GetTransform()->Scale(D3D::Width(), D3D::Height(), 1);
	render2D->GetTransform()->Position(D3D::Width() * 0.5f, D3D::Height() * 0.5f, 0);

	texture = new Texture(L"Environment/Bridge.jpg");
	//render2D->SRV(texture->SRV());


	Shader* shader = new Shader(L"29_Texture.fx");
	textureBuffer = new Texture2DBuffer(texture->GetTexture());
	
	shader->AsSRV("Input")->SetResource(textureBuffer->SRV());
	shader->AsUAV("Output")->SetUnorderedAccessView(textureBuffer->UAV());


	UINT width = textureBuffer->Width();
	UINT height = textureBuffer->Height();
	UINT page = textureBuffer->Page();

	float x = ((float)width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	float y = ((float)height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);

	shader->Dispatch(0, 0, (UINT)ceilf(x), (UINT)ceilf(y), page);

	render2D->SRV(textureBuffer->OuputSRV());
}
