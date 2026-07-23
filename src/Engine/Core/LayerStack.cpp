#include "LayerStack.h"

LayerStack* LayerStack::m_pInstance = nullptr;

LayerStack::LayerStack()
{
	m_pInstance = this;
}

void LayerStack::PushLayer_(Engine::Layer *pLayer)
{
	m_pInstance->m_Layers.push_back(pLayer);
}

void LayerStack::Setup()
{
	for (auto* l : m_Layers)
	{
		l->Setup();
	}
}

void LayerStack::Destroy()
{
	for (auto* l : m_Layers)
	{
		l->Destroy();
	}
}

void LayerStack::OnUpdate(double mDelta)
{
	for (auto* l : m_Layers)
	{
		l->OnUpdate(mDelta);
	}
}

void LayerStack::OnEvent(Event& event)
{
	for (int i = (m_Layers.size() - 1); i >= 0; i--)
	{
		m_Layers[i]->OnEvent(event);
	}
}

void LayerStack::OnImGuiRender()
{
	for (auto* l : m_Layers)
	{
		l->OnImGuiRender();
	}
}
