#ifndef VKCREATIONS_H
#define VKCREATIONS_H

#include "vkConfig.h"
#include "vkLog.h"

namespace GPU
{

	VkInstance CreateInstance(uint32_t mInstanceVersion, uint32_t mAppVersion, uint32_t mEngineVersion);

}

#endif