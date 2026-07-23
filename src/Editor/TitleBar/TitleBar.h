#pragma once

#include <Engine.h>

class TitleBar : public Engine::Layer
{
public:
    TitleBar(const char* mName = "Editor::TitleBar") : Engine::Layer(mName) {}

    virtual void Setup() override;
	virtual void Destroy() override;

	virtual void OnUpdate(double mDelta) override;
	virtual void OnEvent(Event& event) override;
	virtual void OnImGuiRender() override;

private:
	//UI::Texture* mTestTex;
};