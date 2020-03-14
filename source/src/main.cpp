#include "hanoRenderer.hpp"

int main()
{
	{
		hano::Renderer renderer;

		hano::Scene scene;
		scene.meshes.emplace_back("assets/obj/cube.obj");
		
		renderer.setRenderScene(scene);

		while (renderer.isRunning())
		{
			renderer.renderFrame();
		}
	}
    return 0;
}