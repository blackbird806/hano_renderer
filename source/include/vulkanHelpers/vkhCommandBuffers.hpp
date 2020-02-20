#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "vkhUtility.hpp"
#include "vkhCommandPool.hpp"

namespace hano::vkh
{
	struct CommandBuffers
	{
		VULKAN_NON_COPIABLE(CommandBuffers);

		CommandBuffers(CommandPool const& pool, uint32 size)
			: commandPool(pool)
		{
			vk::CommandBufferAllocateInfo allocInfo = {};
			allocInfo.commandPool = commandPool.handle;
			allocInfo.level = vk::CommandBufferLevel::ePrimary;
			allocInfo.commandBufferCount = size;

			commandBuffers = commandPool.device.handle.allocateCommandBuffers(allocInfo);
		}

		~CommandBuffers()
		{
			if (!commandBuffers.empty())
			{
				commandPool.device.handle.freeCommandBuffers(commandPool.handle, commandBuffers);
				commandBuffers.clear();
			}
		}

		HANO_NODISCARD vk::CommandBuffer begin(size_t i)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr; // Optional
			commandBuffers[i].begin(beginInfo);
			
			return commandBuffers[i];
		}

		void end(size_t i)
		{
			commandBuffers[i].end();
		}
		
		HANO_NODISCARD uint32 size() const noexcept { return static_cast<uint32>(commandBuffers.size()); }
		HANO_NODISCARD vk::CommandBuffer& operator [] (const size_t i) noexcept { return commandBuffers[i]; }
		
		CommandPool const& commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;
	};

	struct SingleTimeCommands
	{
		SingleTimeCommands(CommandPool& pool)
			: buffer(pool, 1), commandPool(pool)
		{

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(buffer[0], &beginInfo);
		}

		void end()
		{
			vkEndCommandBuffer(buffer[0]);

			vk::SubmitInfo submitInfo = {};
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &buffer[0];

			const auto graphicsQueue = commandPool.device.graphicsQueue;
			VKH_CHECK(
				graphicsQueue.submit(1, &submitInfo, nullptr), "failed to submit single time Command !");
			graphicsQueue.waitIdle();
		}

		~SingleTimeCommands()
		{
			end();
		}
		
		CommandBuffers buffer;
		CommandPool& commandPool;
	};
}
