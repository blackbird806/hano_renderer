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

	vk::ApplicationInfo appInfo = {};
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = engineName;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	vk::InstanceCreateInfo createInfo = {};
	createInfo.pApplicationInfo = &appInfo;

	auto glfwExtensions = getGlfwRequiredExtensions(validationLayersEnabled());

	createInfo.enabledExtensionCount = static_cast<uint32>(glfwExtensions.size());
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();

	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

	if (validationLayersEnabled())
	{
		createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		debugCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
		debugCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
		debugCreateInfo.pfnUserCallback = &debugVkCallback;
		debugCreateInfo.pUserData = nullptr;

		vk::ValidationFeatureEnableEXT enables[] = { vk::ValidationFeatureEnableEXT::eBestPractices };
		vk::ValidationFeaturesEXT features = {};
		features.enabledValidationFeatureCount = std::size(enables);
		features.pEnabledValidationFeatures = enables;
		debugCreateInfo.pNext = &features;
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
	std::vector<vk::LayerProperties> availablesLayers = vk::enumerateInstanceLayerProperties();
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