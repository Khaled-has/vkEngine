#include "vkShader.h"
#include "vkContext.h"
#include "vkLog.h"

#include <fstream>
#include <shaderc.hpp>

namespace GPU
{
	std::vector<uint32_t> ReadFile(const std::string& mPath)
	{
		std::ifstream file(mPath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			GPU_LOG_ERROR("vkGPU: Failed to open the shader file: {0}", mPath);
		}

		size_t pSize = file.tellg();
		std::vector<uint32_t> buffer(pSize / 4);

		file.seekg(0);
		file.read(reinterpret_cast<char*>(buffer.data()), pSize);
		file.close();

		return buffer;
	}

	std::string ReadFileString(const std::string& mPath)
	{
		std::ifstream file(mPath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			GPU_LOG_ERROR("vkGPU: Failed to open the shader file: {0}", mPath);
		}

		size_t pSize = file.tellg();
		std::string buffer(pSize, '\0');

		file.seekg(0);
		file.read(buffer.data(), pSize);
		file.close();

		return buffer;
	}

	std::vector<uint32_t> CompileShader(
		const std::string& source,
		shaderc_shader_kind kind,
		const std::string& name
	)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		auto result = compiler.CompileGlslToSpv(
			source,
			kind,
			name.c_str(),
			options
		);

		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			GPU_LOG_ERROR("vkGPU: Shader error: {0}", result.GetErrorMessage());
		}

		return { result.cbegin(), result.cend() };
	}

	VkShaderModule CreateShaderModuleFromBinary(std::string pFileName, shaderc_shader_kind pKind)
	{
		// # Step 1: Read spv
		std::string pFinalPath = std::string(RES_PATH) + pFileName;
		std::string pSource = ReadFileString(pFinalPath);

		// # Step 2: Compile spv
		std::vector<uint32_t> spv = CompileShader(
			pSource,
			pKind,
			pFileName
		);

		// # Step 3: Create the shader
		VkShaderModuleCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = size_t(spv.size() * sizeof(uint32_t)),
			.pCode = spv.data()
		};

		VkShaderModule pShaderModule = VK_NULL_HANDLE;
		VkResult res = vkCreateShaderModule(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &CreateInfo,
			NULL, &pShaderModule
		);

		return pShaderModule;
	}

	VkShaderModule CreateShaderModuleDirctly(std::string pSource, shaderc_shader_kind pKind)
	{
		// # Step 1: Compile spv
		std::vector<uint32_t> spv = CompileShader(
			pSource,
			pKind,
			pSource
		);

		// # Step 2: Create the shader
		VkShaderModuleCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = size_t(spv.size() * sizeof(uint32_t)),
			.pCode = spv.data()
		};

		VkShaderModule pShaderModule = VK_NULL_HANDLE;
		VkResult res = vkCreateShaderModule(
			GetCurrentContext()->getDeviceRes().pDevice->getHandle(), &CreateInfo,
			NULL, &pShaderModule
		);
		/*VK_CHECK("vkCreateShaderModule", res);*/

		return pShaderModule;
	}


	void vkShader::CreateFromText(const char* pVertexShader, const char* pFragmentShader)
	{
		m_VertexModule = CreateShaderModuleDirctly(pVertexShader, shaderc_vertex_shader);
		m_FragmentModule = CreateShaderModuleDirctly(pFragmentShader, shaderc_fragment_shader);
	}

	void vkShader::Create(const char* pVertexPath, const char* pFragmentPath)
	{
		m_VertexModule = CreateShaderModuleFromBinary(pVertexPath, shaderc_vertex_shader);
		m_FragmentModule = CreateShaderModuleFromBinary(pFragmentPath, shaderc_fragment_shader);
	}
}