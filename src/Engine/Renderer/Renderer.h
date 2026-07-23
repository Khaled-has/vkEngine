#pragma once

class Renderer
{
public:
	Renderer() {}

	virtual void Initialize() = 0;
	virtual void Destroy() = 0;

	virtual void Rendering() = 0;
};

Renderer* CreateRenderer();