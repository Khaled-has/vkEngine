#ifndef VK_SHADER_H
#define VK_SHADER_H

#include <volk.h>

class VK_Shader
{
public:
	VK_Shader() {}

	void Create(const char* pVertexPath, const char* pFragmentPath);
	void CreateFromText(const char* pVertexShader, const char* pFragmentShader);

	inline const VkShaderModule& getVertexShader() const { return m_pVertexSh; }
	inline const VkShaderModule& getFragmentShader() const { return m_pFragmentSh; }

private:
	VkShaderModule m_pVertexSh = VK_NULL_HANDLE;
	VkShaderModule m_pFragmentSh = VK_NULL_HANDLE;
};

#endif