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
	hano::Mesh& house = renderer.loadMesh("assets/obj/rungholt.obj");
	hano::Model simpleCube(cube);
	hano::Model simpleHouse(house);

	hano::Scene scene;
	scene.addModel(simpleCube);
	simpleCube.transform.pos = glm::vec3(0, 0, 2);

	scene.addModel(simpleHouse);
	simpleHouse.transform.pos = glm::vec3(1, 0, 20);

	scene.addLight(hano::PointLight{ 1.0f, glm::vec3(2.0f, 1.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f) });

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