#include "Layer.h"
#include "LayerStack.h"

namespace Engine
{
	void PushLayer(Layer* pLayer)
	{
		LayerStack::PushLayer_(pLayer);
	}
}