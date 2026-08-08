#pragma once

#include <Layer.h>

#include "UIComponents/TransformUI.h"

class Inspector : public Engine::Layer
{
public:
	Inspector(int* pSelectedItem, const char* pName = "Editor::Inspector") : Engine::Layer(pName) { m_pSelectedItem = pSelectedItem; }

	virtual void Setup() override;
	virtual void Destroy() override;

	virtual void OnUpdate(double mDelta) override {}; // Unused
	virtual void OnEvent(Event& event) override {};   // Unused
	virtual void OnImGuiRender() override;

private:
	int* m_pSelectedItem = nullptr;

	UIComponent::TransformUI mTransformUI;
};