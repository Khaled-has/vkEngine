#include "UI.h"

#include <vkGPU.h>

namespace UI
{
	static UIContext* pUICxt = nullptr;

	void CreateContext()
	{
		pUICxt = new UIContext{
			.pvolkProcAddress = vkGetInstanceProcAddr,
			.pDevice = VK_Device::Get()
		};
	}

	void SetUIContext(UIContext* pUIContext)
	{
		pUICxt = pUIContext;

		VK_Device* device = reinterpret_cast<VK_Device*>(pUICxt->pDevice);

		volkInitializeCustom(reinterpret_cast<PFN_vkGetInstanceProcAddr>(pUIContext->pvolkProcAddress));
		volkLoadInstance(device->getInstance());
		volkLoadDevice(device->getDevice());

		VK_Device::Set(device);

		if (!volkGetLoadedInstance())
		{
			std::cout << "Error in volk\n";
			exit(1);
		}
		else std::cout << "Success in volk\n";
		
	}

	UIContext* GetUIContext()
	{
		return pUICxt;
	}
}
