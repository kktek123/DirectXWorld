#pragma once
#include "Systems/IExecute.h"

class WeatherDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override {}
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	Shader* shader;
	Terrain* terrain;

	CubeSky* sky;


	enum class WeatherType
	{
		None = 0, Rain, Snow
	} weatherType;

	class Rain* rain;
	class Snow* snow;
};