#include "model.hpp"

using namespace hano;

Model::Model(Mesh const& mesh) 
	: resourceMesh(&mesh), resourceMaterial(nullptr)
{
	
}

Model::Model(Mesh const& mesh, Material const& mtrl)
	: resourceMesh(&mesh), resourceMaterial(&mtrl)
{

}

void Model::render(vk::CommandBuffer commandBuffer)
{
	resourceMesh->render(commandBuffer);
}

Mesh const& Model::getMesh() const
{
	assert(resourceMesh);
	return *resourceMesh;
}

Material const& Model::getMaterial() const
{
	assert(resourceMaterial);
	return *resourceMaterial;
}
