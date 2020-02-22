#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "vkhUtility.hpp"
#include "vkhCommandPool.hpp"

namespace hano::vkh
{
	struct CommandBuffers
	{
		VULKAN_NON_COPIABLE_NON_MOVABLE(CommandBuffers);

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
		VULKAN_NON_COPIABLE_NON_MOVABLE(SingleTimeCommands);
		
		SingleTimeCommands(CommandPool& pool)
			: commandPool(pool), commandBuffers(pool, 1)
		{
			vk::CommandBufferBeginInfo beginInfo = {};
			beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
			
			commandBuffers[0].begin(beginInfo);
		}

		operator vk::CommandBuffer()
		{
			return commandBuffers[0];
		}

		vk::CommandBuffer& buffer()
		{
			return commandBuffers[0];
		}
		
		void end()
		{
			vkEndCommandBuffer(commandBuffers[0]);

			vk::SubmitInfo submitInfo = {};
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffers[0];

			const auto graphicsQueue = commandPool.device.graphicsQueue();
			VKH_CHECK(
				graphicsQueue.submit(1, &submitInfo, nullptr), "failed to submit single time Command !");
			graphicsQueue.waitIdle();
		}

		~SingleTimeCommands()
		{
			end();
		}
		
		CommandPool& commandPool;
	private:
		CommandBuffers commandBuffers;
	};
}
