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

	hano::Scene scene;
	scene.addModel(simpleCube);

	renderer.setRenderScene(scene);

	hano::HanoEditor editor(renderer);
	renderer.setEditorGUI(editor);

	while (renderer.isRunning())
	{
		renderer.renderFrame();
	}

	renderer.destroy();
	
    return 0;
}