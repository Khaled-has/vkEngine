#pragma once

#include <vector>

#include "Layer.h"

class LayerStack
{
public:
	LayerStack();

	static void PushLayer_(Engine::Layer* pLayer);

	void Setup();
	void Destroy();

	void OnUpdate(double mDelta);
	void OnEvent(Event& event);
	void OnImGuiRender();

private:
	static LayerStack* m_pInstance;
	std::vector<Engine::Layer*> m_Layers;
};