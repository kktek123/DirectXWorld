#pragma once
#include "Systems/IExecute.h"

class ComputeDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	void Raw();
	void Intersection();

	void Texture2D();

private:
	Render2D* render2D = NULL;
	Texture* texture;
	Texture2DBuffer* textureBuffer;
};