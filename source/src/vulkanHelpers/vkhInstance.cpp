#include <vulkanHelpers/vkhInstance.hpp>

using namespace hano::vkh;

std::vector<const char*> hano::vkh::getGlfwRequiredExtensions(bool validationLayersEnabled)
{
	uint32 glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// @TODO handle this in an elegant way
	// Needed to enable VK_NV_RAY_TRACING_EXTENSION_NAME
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	if (validationLayersEnabled)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

Instance::Instance(const char* appName, const char* engineName, std::vector<const char*> validationLayers_, vk::AllocationCallbacks* alloc, GLFWwindow* win)
	: allocator(alloc), validationLayers(validationLayers_), window(win)
{
	assert(window);
	// disable validation layers if none provided
	if (validationLayersEnabled() && !checkValidationLayerSupport())
		throw hano::HanoException("validation layers requested, but not available!");

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = engineName;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto glfwExtensions = getGlfwRequiredExtensions(validationLayersEnabled());

	createInfo.enabledExtensionCount = static_cast<uint32>(glfwExtensions.size());
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

	if (validationLayersEnabled())
	{
		createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = &debugVkCallback;
		debugCreateInfo.pUserData = nullptr;
		createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	handle = vk::createInstanceUnique(createInfo, allocator);

	physicalDevices = handle.get().enumeratePhysicalDevices();
	extensions = vk::enumerateInstanceExtensionProperties(nullptr);
}

bool Instance::validationLayersEnabled() const noexcept
{
	return !validationLayers.empty();
}

bool Instance::checkValidationLayerSupport()
{
	uint32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availablesLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availablesLayers.data());

	for (auto const& layerName : validationLayers)
	{
		bool layerFound = false;
		for (auto const& layerProperties : availablesLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			return false;
	}
	return true;
}