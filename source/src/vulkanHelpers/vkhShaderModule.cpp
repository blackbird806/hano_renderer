#include <vulkanHelpers/vkhShaderModule.hpp>
#include <vulkanHelpers/vkhDevice.hpp>
#include <core/hanoFile.hpp>

using namespace hano::vkh;

ShaderModule::ShaderModule(Device const& idevice, std::vector<char> const& code)
	: device(&idevice)
{
	vk::ShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.codeSize = code.size();
	shaderInfo.pCode = reinterpret_cast<uint32_t const*>(code.data());
	
	handle = device->handle.createShaderModuleUnique(shaderInfo, device->allocator);
}

ShaderModule::ShaderModule(Device const& idevice, std::string const& filename)
	: ShaderModule(idevice, readFile(filename))
{
}

vk::PipelineShaderStageCreateInfo ShaderModule::createShaderStageInfo(vk::ShaderStageFlagBits stageBits) const
{
	vk::PipelineShaderStageCreateInfo createInfo = {};
	createInfo.stage = stageBits;
	createInfo.module = handle.get();
	createInfo.pName = "main";

	return createInfo;
}
