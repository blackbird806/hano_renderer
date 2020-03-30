#include <cassert>
#include "vulkanHelpers\vkhDebugUtilsExtension.hpp"

static PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT		= nullptr;
static PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT			= nullptr;
static PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT	= nullptr;
static PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT		= nullptr;
static PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT	= nullptr;
static PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT		= nullptr;
static PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT			= nullptr;
static PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT		= nullptr;
static PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT	= nullptr;
static PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT	= nullptr;
static PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT		= nullptr;

VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectNameEXT(
	VkDevice                                    device,
	const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
{
	assert(pfn_vkSetDebugUtilsObjectNameEXT);
	return pfn_vkSetDebugUtilsObjectNameEXT(device, pNameInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectTagEXT(
	VkDevice                                    device,
	const VkDebugUtilsObjectTagInfoEXT* pTagInfo)
{
	assert(pfn_vkSetDebugUtilsObjectTagEXT);
	return pfn_vkSetDebugUtilsObjectTagEXT(device, pTagInfo);
}
VKAPI_ATTR void VKAPI_CALL vkQueueBeginDebugUtilsLabelEXT(
	VkQueue                                     queue,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkQueueBeginDebugUtilsLabelEXT);
	pfn_vkQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
}
VKAPI_ATTR void VKAPI_CALL vkQueueEndDebugUtilsLabelEXT(
	VkQueue                                     queue)
{
	assert(pfn_vkQueueEndDebugUtilsLabelEXT);
	pfn_vkQueueEndDebugUtilsLabelEXT(queue);
}
VKAPI_ATTR void VKAPI_CALL vkQueueInsertDebugUtilsLabelEXT(
	VkQueue                                     queue,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkQueueInsertDebugUtilsLabelEXT);
	pfn_vkQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBeginDebugUtilsLabelEXT(
	VkCommandBuffer                             commandBuffer,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkCmdBeginDebugUtilsLabelEXT);
	pfn_vkCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdEndDebugUtilsLabelEXT(
	VkCommandBuffer                             commandBuffer)
{
	assert(pfn_vkCmdEndDebugUtilsLabelEXT);
	pfn_vkCmdEndDebugUtilsLabelEXT(commandBuffer);
}
VKAPI_ATTR void VKAPI_CALL vkCmdInsertDebugUtilsLabelEXT(
	VkCommandBuffer                             commandBuffer,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkCmdInsertDebugUtilsLabelEXT);
	pfn_vkCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance                                  instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pMessenger)
{
	assert(pfn_vkCreateDebugUtilsMessengerEXT);
	return pfn_vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
	VkInstance                                  instance,
	VkDebugUtilsMessengerEXT                    messenger,
	const VkAllocationCallbacks* pAllocator)
{
	assert(pfn_vkDestroyDebugUtilsMessengerEXT);
	pfn_vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
VKAPI_ATTR void VKAPI_CALL vkSubmitDebugUtilsMessageEXT(
	VkInstance                                  instance,
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	assert(pfn_vkSubmitDebugUtilsMessageEXT);
	pfn_vkSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
}

void hano::load_VK_EXT_debug_utils(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr) noexcept
{
	pfn_vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)getDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
	pfn_vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)getDeviceProcAddr(device, "vkSetDebugUtilsObjectTagEXT");
	pfn_vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkQueueBeginDebugUtilsLabelEXT");
	pfn_vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkQueueEndDebugUtilsLabelEXT");
	pfn_vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkQueueInsertDebugUtilsLabelEXT");
	pfn_vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkCmdBeginDebugUtilsLabelEXT");
	pfn_vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkCmdEndDebugUtilsLabelEXT");
	pfn_vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkCmdInsertDebugUtilsLabelEXT");
	pfn_vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)getInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	pfn_vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)getInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	pfn_vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)getInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
	
	assert(pfn_vkSetDebugUtilsObjectNameEXT		!= nullptr);
	assert(pfn_vkSetDebugUtilsObjectTagEXT		!= nullptr);
	assert(pfn_vkQueueBeginDebugUtilsLabelEXT	!= nullptr);
	assert(pfn_vkQueueEndDebugUtilsLabelEXT		!= nullptr);
	assert(pfn_vkQueueInsertDebugUtilsLabelEXT	!= nullptr);
	assert(pfn_vkCmdBeginDebugUtilsLabelEXT		!= nullptr);
	assert(pfn_vkCmdEndDebugUtilsLabelEXT		!= nullptr);
	assert(pfn_vkCmdInsertDebugUtilsLabelEXT	!= nullptr);
	assert(pfn_vkCreateDebugUtilsMessengerEXT	!= nullptr);
	assert(pfn_vkDestroyDebugUtilsMessengerEXT	!= nullptr);
	assert(pfn_vkSubmitDebugUtilsMessageEXT		!= nullptr);
}
