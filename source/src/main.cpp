#include "hanoRenderer.hpp"

int main()
{
	{
		hano::Renderer renderer;
		hano::Scene scene;
		scene.camera.pos = glm::vec3(0, 0, -3);
		//scene.meshes.emplace_back(renderer.m_vkContext, "assets/obj/Sniper.obj");
		scene.meshes.emplace_back(renderer.m_vkContext, "assets/obj/cube.obj");
		renderer.setRenderScene(scene);

		while (renderer.isRunning())
		{
			renderer.renderFrame();
		}

		// @TODO need to wait work finishe before deinit vulkan data
		renderer.m_vkContext.device->handle.waitIdle();
	}
    return 0;
}