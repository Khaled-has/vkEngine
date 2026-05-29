#ifndef CONFIG_H
#define CONFIG_H

#include "log.h"

#define CHECK_VK_RES(_func, _messege) if (_func != VK_SUCCESS) { LOG_ERROR("line ( {0} ) :Vulkan Error: {1}", __LINE__, _messege); } 

#endif