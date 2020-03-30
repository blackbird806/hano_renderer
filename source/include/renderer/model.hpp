#pragma once
#include "mesh.hpp"
#include "material.hpp"

namespace hano
{
	class Model
	{
	public:
		
		Model(Mesh const& mesh);
		Model(Mesh const& mesh, Material const& mtrl);
		Model(Model&&) noexcept = default;
		Model& operator=(Model&&) noexcept = default;
		
		void render(vk::CommandBuffer commandBuffer);

	private:

		std::vector<vkh::Buffer> m_uniformBuffers;
		vkh::DescriptorSets m_descriptorSets;
		Mesh const* resourceMesh;
		Material const* resourceMaterial;
	};
}