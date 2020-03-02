#include <vulkanContext.hpp>

using namespace hano;

void VulkanContext::init(GLFWwindow* window, VulkanConfig const& config)
{
	assert(window);
	instance = std::make_unique<vkh::Instance>(config.appName, config.engineName,
		std::vector(c_vulkanValidationLayers.begin(), c_vulkanValidationLayers.end()),
		vkAllocator, window);

	surface = std::make_unique<vkh::Surface>(*instance, window, vkAllocator);
	device = std::make_unique<vkh::Device>(getSuitableDevice(), *surface, std::vector(c_vulkanDefaultRequiredExtentions.begin(), c_vulkanDefaultRequiredExtentions.end()), vkAllocator);
	swapchain = std::make_unique<vkh::Swapchain>(*device, config.vsyncEnabled);
	commandPool = std::make_unique<vkh::CommandPool>(*device, device->graphicsFamilyIndex(), true);
	// commandBuffers = std::make_unique<vkh::CommandBuffers>(*commandPool, );
	depthBuffer = std::make_unique<vkh::DepthBuffer>(*commandPool, swapchain->extent);
}

std::vector<vk::PhysicalDevice> const& hano::VulkanContext::getPhysicalDevices() const
{
	return instance->physicalDevices;
}

std::vector<vk::ExtensionProperties> const& hano::VulkanContext::getExtensions() const
{
	return instance->extensions;
}

vk::PhysicalDevice VulkanContext::getSuitableDevice() const
{
	auto const& physicalDevices = getPhysicalDevices();
	auto const result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](vk::PhysicalDevice const& device)
		{
			// We want a device with geometry shader support.
			vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

			if (!deviceFeatures.geometryShader)
			{
				return false;
			}

			// We want a device with a graphics queue.
			auto const queueFamilies = device.getQueueFamilyProperties();

			auto const hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const vk::QueueFamilyProperties& queueFamily)
				{
					return queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics;
				});

			return hasGraphicsQueue != queueFamilies.end();
		});

	if (result == physicalDevices.end())
		throw HanoException("cannot find a suitable device !");

	return *result;
}
