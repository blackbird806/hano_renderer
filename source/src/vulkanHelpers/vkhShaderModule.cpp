#include <vulkanHelpers/vkhShaderModule.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <core/hanoFile.hpp>

using namespace hano::vkh;

ShaderModule::ShaderModule(Device const& idevice, std::vector<char> const& code, vk::ShaderStageFlagBits shaderStage_)
	: device(&idevice), shaderStage(shaderStage_)
{
	vk::ShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.codeSize = code.size();
	shaderInfo.pCode = reinterpret_cast<uint32_t const*>(code.data());
	
	handle = device->handle.createShaderModuleUnique(shaderInfo, device->allocator);
}

ShaderModule::ShaderModule(Device const& idevice, std::string const& filename, vk::ShaderStageFlagBits shaderStage_)
	: ShaderModule(idevice, readFile(filename), shaderStage_)
{
}

vk::PipelineShaderStageCreateInfo ShaderModule::createShaderStageInfo() const
{
	vk::PipelineShaderStageCreateInfo createInfo = {};
	createInfo.stage = shaderStage;
	createInfo.module = handle.get();
	createInfo.pName = "main";

	return createInfo;
}
