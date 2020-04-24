#include <hanoRenderer.hpp>
#include <renderer/hanoEditor.hpp>
#include <renderer/gltfLoader.hpp>

int main()
{
	hano::Renderer::Info infos = {
		.appName = "Render test 1",
		.windowWidth = 1280,
		.windowHeight = 720
	};

	hano::Renderer renderer(infos);

	//hano::Mesh& cube = renderer.loadMesh("assets/gltf/cube/Cube.gltf");
	//hano::Material houseMtrl{ .baseColor = &renderer.loadTexture("assets/textures/back.jpg") };

	auto gltfCube = hano::loadGltfModel(renderer, "assets/gltf/DamagedHelmet.gltf");
	hano::Model simpleCube(gltfCube.mesh, hano::Material{.baseColor = &gltfCube .textures[0]});

	hano::Scene scene;
	scene.camera.pos = glm::vec3(0, 0, -10.0f);
	scene.addModel(simpleCube);
	simpleCube.transform.pos = glm::vec3(0, 0, 2);

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