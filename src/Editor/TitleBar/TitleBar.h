#pragma once

#include <Engine.h>
#include <UI.h>

class TitleBar : public Engine::Layer
{
public:
    TitleBar(const char* pName = "Editor::TitleBar") : Engine::Layer(pName) {}

    virtual void Setup() override;
	virtual void Destroy() override;

	virtual void OnUpdate(double mDelta) override;
	virtual void OnEvent(Event& event) override;
	virtual void OnImGuiRender() override;

private:
	UI::Texture mIconTex;
	UI::Texture mTBarTex;

	void CustumTBar();

	void FileMenu();
	void EditeMenu();

	void WindowButtons();
};