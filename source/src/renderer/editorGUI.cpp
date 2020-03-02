#include <renderer/editorGUI.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <core/hanoException.hpp>
#include <vulkanHelpers/vkhDescriptorBinding.hpp>

using namespace hano;

namespace 
{
	void checkVulkanResultCallback(const VkResult err)
	{
		if (err != VK_SUCCESS)
			HanoException(std::string("ImGui Vulkan error (") + to_string(vk::Result(err)) + ")");
	}
}

EditorGUI::EditorGUI(VulkanContext const& vkContext_)
	: vkContext(&vkContext_)
{
	const auto& device = vkContext->device;
	const auto& window = vkContext->surface->instance.window;

	// Initialise descriptor pool and render pass for ImGui.
	std::vector<vkh::DescriptorBinding> const descriptorBindings =
	{
		{0, 1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlags()},
	};

	descriptorPool = std::make_unique<vkh::DescriptorPool>(device, descriptorBindings, 1);
	renderPass = std::make_unique<vkh::RenderPass>(vkContext->swapchain, vkContext->depthBuffer, false, false);

	// Initialise ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Initialise ImGui GLFW adapter
	if (!ImGui_ImplGlfw_InitForVulkan(window, false))
		throw HanoException("failed to initialise ImGui GLFW adapter");

	// Initialise ImGui Vulkan adapter
	ImGui_ImplVulkan_InitInfo vulkanInit = {};
	vulkanInit.Instance = device->surface.instance.handle;
	vulkanInit.PhysicalDevice = device->physicalDevice;
	vulkanInit.Device = device->handle;
	vulkanInit.QueueFamily = device->graphicsFamilyIndex();
	vulkanInit.Queue = device->graphicsQueue();
	vulkanInit.PipelineCache = nullptr;
	vulkanInit.DescriptorPool = descriptorPool->handle.get();
	vulkanInit.MinImageCount = vkContext->swapchain->minImageCount;
	vulkanInit.ImageCount = static_cast<uint32_t>(vkContext->swapchain->images.size());
	vulkanInit.Allocator = nullptr;
	vulkanInit.CheckVkResultFn = checkVulkanResultCallback;

	if (!ImGui_ImplVulkan_Init(&vulkanInit, renderPass->handle.get()))
		HanoException("failed to initialise ImGui vulkan adapter");

	auto& io = ImGui::GetIO();
	// No ini file.
	io.IniFilename = nullptr;
	// Window scaling and style.
	float xscale;
	float yscale;
	glfwGetWindowContentScale(window, &xscale, &yscale);
	const auto scaleFactor = xscale;

	ImGui::StyleColorsDark();
	ImGui::GetStyle().ScaleAllSizes(scaleFactor);

	// Upload ImGui fonts
	//if (!io.Fonts->AddFontFromFileTTF("../assets/fonts/Cousine-Regular.ttf", 13 * scaleFactor))
	//	HanoException("failed to load ImGui font");

	{
		vkh::SingleTimeCommands commands(*vkContext->commandPool);
		if (!ImGui_ImplVulkan_CreateFontsTexture(commands.buffer()))
			HanoException("failed to create ImGui font textures");
	}

	//ImGui_ImplVulkan_DestroyFontUploadObjects();
}

EditorGUI::~EditorGUI()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorGUI::Render(vk::CommandBuffer commandBuffer, vkh::FrameBuffer const& framebuffer)
{

}