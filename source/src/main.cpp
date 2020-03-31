#include "hanoRenderer.hpp"
#include "renderer/hanoEditor.hpp"

int main()
{
	hano::Renderer::Info infos = {
		.appName = "Render test 1",
		.windowWidth = 800,
		.windowHeight = 600
	};

	hano::Renderer renderer(infos);

	hano::Mesh& cube = renderer.loadMesh("assets/obj/cube.obj");
	hano::Model simpleCube(cube);
	hano::Model simpleCube2(cube);

	hano::Scene scene;
	scene.addModel(simpleCube);
	simpleCube.transform.pos = glm::vec3(0, 0, -5);
	simpleCube.transform.scale = glm::vec3(0.1, 0.1, 0.1);

	scene.addModel(simpleCube2);
	simpleCube2.transform.pos = glm::vec3(0, 0, 5);
	simpleCube2.transform.scale = glm::vec3(0.1, 0.1, 0.1);

	renderer.setRenderScene(scene);

	hano::HanoEditor editor(renderer);
	renderer.setEditorGUI(editor);

	while (renderer.isRunning())
	{
		renderer.renderFrame();
	}

	renderer.waitIdle();
	
    return 0;
}