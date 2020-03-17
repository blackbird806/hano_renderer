#include "hanoRenderer.hpp"

int main()
{
	{
		hano::Renderer renderer;
		hano::Scene scene;
		scene.meshes.emplace_back(renderer.m_vkContext, "assets/obj/cube.obj");
		//scene.meshes.emplace_back(renderer.m_vkContext, "assets/obj/cube.obj");
		renderer.setRenderScene(scene);

		while (renderer.isRunning())
		{
			renderer.renderFrame();
		}
	}
    return 0;
}