#include "VK_Shader.h"

#include "config.h"
#include "VK_Device.h"

#include <fstream>

#include <shaderc.hpp>

std::vector<uint32_t> ReadFile(const std::string& pPath)
{
	std::ifstream file(pPath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		LOG_ERROR("Shader: Failed to open file");
	}

	size_t pSize = file.tellg();
	std::vector<uint32_t> buffer(pSize / 4);

	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()), pSize);
	file.close();

	return buffer;
}

std::string ReadFileString(const std::string& pPath)
{
	std::ifstream file(pPath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		LOG_ERROR("Shader: Failed to open file");
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
		LOG_ERROR("Shader Error: {0}", result.GetErrorMessage());
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
		VK_Device::Get()->getDevice(), &CreateInfo,
		NULL, &pShaderModule
	);
	//VK_CHECK("vkCreateShaderModule", res);

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
		VK_Device::Get()->getDevice(), &CreateInfo,
		NULL, &pShaderModule
	);
	/*VK_CHECK("vkCreateShaderModule", res);*/

	return pShaderModule;
}


//void VK_Shader::InitFromText(const char* pVertexShader, const char* pFragmentShader)
//{
//	pVS = CreateShaderModuleDirctly(pVertexShader, shaderc_vertex_shader);
//	pFS = CreateShaderModuleDirctly(pFragmentShader, shaderc_fragment_shader);
//}

void VK_Shader::Create(const char* pVertexPath, const char* pFragmentPath)
{
	m_pVertexSh = CreateShaderModuleFromBinary(pVertexPath, shaderc_vertex_shader);
	m_pFragmentSh = CreateShaderModuleFromBinary(pFragmentPath, shaderc_fragment_shader);
}
