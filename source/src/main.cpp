#include "hanoRenderer.hpp"

int main()
{
	{
		hano::Renderer renderer;
		hano::Scene scene;
		scene.meshes.emplace_back(renderer.m_vkContext, "assets/obj/cube.obj");
		scene.meshes[0].updateUniformBuffer(0);
		scene.meshes[0].updateUniformBuffer(1);
		renderer.setRenderScene(scene);

		while (renderer.isRunning())
		{
			renderer.renderFrame();
		}
	}
    return 0;
}