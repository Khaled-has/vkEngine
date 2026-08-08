#ifndef VKSHADER_H
#define VKSHADER_H

#include "vkConfig.h"

namespace GPU
{
	class VKGPU_API vkShader
	{
	public:
		vkShader() {}
		void Create(const char* pVertexPath, const char* pFragmentPath);
		void CreateFromText(const char* pVertexShader, const char* pFragmentShader);

		inline const VkShaderModule& getVertexModule() const { return m_VertexModule; }
		inline const VkShaderModule& getFragmentModule() const { return m_FragmentModule; }

	private:
		VkShaderModule m_VertexModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentModule = VK_NULL_HANDLE;
	};
}

#endif