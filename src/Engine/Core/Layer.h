#pragma once

#include <string>

#include "EngineAPI.h"
#include "Event.h"

namespace Engine
{
	class VK_ENGINE_API Layer
	{
	public:
		Layer(const char* mName) : m_name(mName) {}

		virtual void Setup() = 0;
		virtual void Destroy() = 0;

		virtual void OnUpdate(double mDelta) = 0;
		virtual void OnEvent(Event& event) = 0;
		virtual void OnImGuiRender() = 0;

		inline std::string getName() const { return m_name; }
	private:
		std::string m_name;
	};

	extern "C" VK_ENGINE_API void PushLayer(Layer* pLayer);
}