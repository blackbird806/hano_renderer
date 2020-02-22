#include <glfw/glfw3.h>
#include <hanoRenderer.hpp>
#include <map>

using namespace hano;

Renderer::Renderer()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	
	m_instance = std::make_unique<vkh::Instance>("testApp", "hanoRendererEngine", 
		std::vector(c_vulkanValidationLayers.begin(), c_vulkanValidationLayers.end()),
		m_vkAllocator, m_window);

	m_surface = std::make_unique<vkh::Surface>(*m_instance, m_window, m_vkAllocator);
	m_device = std::make_unique<vkh::Device>(getSuitableDevice(), *m_surface, std::vector(c_vulkanDefaultRequiredExtentions.begin(), c_vulkanDefaultRequiredExtentions.end()), m_vkAllocator);
	m_swapchain = std::make_unique<vkh::Swapchain>(*m_device, true/* @TODO user config */);
	m_commandPool = std::make_unique<vkh::CommandPool>(*m_device, m_device->graphicsFamilyIndex(), true);
	//m_commandBuffers = std::make_unique<vkh::CommandBuffers>(*m_commandPool, )
	m_depthBuffer = std::make_unique<vkh::DepthBuffer>(*m_commandPool, m_swapchain->extent);
}

Renderer::~Renderer()
{
	// order of destruction is important
	m_depthBuffer.reset();
	m_swapchain.reset();
	
	m_commandPool.reset();
	m_device.reset();
	m_surface.reset();
	m_instance.reset();
	
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

std::vector<vk::PhysicalDevice> const& Renderer::getPhysicalDevices() const
{
	return m_instance->physicalDevices;
}

std::vector<vk::ExtensionProperties> const& Renderer::getExtensions() const
{
	return m_instance->extensions;
}

vk::PhysicalDevice Renderer::getSuitableDevice() const
{
	auto const& physicalDevices = getPhysicalDevices();
	auto const result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](vk::PhysicalDevice const& device)
		{
			// We want a device with geometry shader support.
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

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
