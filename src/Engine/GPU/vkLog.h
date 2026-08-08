#ifndef VKLOG_H
#define VKLOG_H

#include <Log.h>

#define GPU_LOG_ERROR(fmt, ...) LOG_ERROR(fmt, __VA_ARGS__)
#define GPU_LOG_WARN(fmt, ...)  LOG_WARN(fmt, __VA_ARGS__)
#define GPU_LOG_INFO(fmt, ...)  LOG_INFO(fmt, __VA_ARGS__)

#define GPU_ASSERT(x, _msg_) ASSERT(x, _msg_)

#define GPU_CHECK(_func_, _msg_) if (_func_ != VK_SUCCESS) { GPU_LOG_ERROR(_msg_); }

#endif