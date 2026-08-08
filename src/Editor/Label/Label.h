#pragma once

#include <Layer.h>
#include <UI.h>

#include <SceneManager.h>
#include <Entity.h>
#include <Components.h>

#include <Viewport.h>

class Label : public Engine::Layer
{
public:
	Label(const char* pName = "Editor::Label") : Engine::Layer(pName) {}

	virtual void Setup() override;
	virtual void Destroy() override;

	virtual void OnUpdate(double mDelta) override {};
	virtual void OnEvent(Event& event) override {};
	virtual void OnImGuiRender() override;

	inline int* getSelectedIndex() { return &m_SelectedEntt; }

private:
	int m_SelectedEntt = -1;
	Render::Viewport* m_Viewport;

	void DrawEntity(Engine::Entity& entity);
};