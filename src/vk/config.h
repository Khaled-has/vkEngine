#ifndef CONFIG_H
#define CONFIG_H

#include <volk.h>

#include "log.h"

#define CHECK_VK_RES(_func, _messege) if (_func != VK_SUCCESS) { LOG_ERROR("line ( {0} ) :Vulkan Error: {1}", __LINE__, _messege); } 

// # ====  vulkan functions  ====

inline VkImageView CreateImageView(const VkImage & Image, VkDevice Device, VkFormat Format, VkImageAspectFlags AspectFlags)
{
	VkImageViewCreateInfo ViewInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = Image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = Format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY,
		},
		.subresourceRange = {
			.aspectMask = AspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	VkImageView ImageView;
	CHECK_VK_RES(vkCreateImageView(Device, &ViewInfo, NULL, &ImageView), "vkCreateImageView");

	return ImageView;
}

#endif