#include <hanoRenderer.hpp>
#include <renderer/hanoEditor.hpp>
#include <renderer/gltfLoader.hpp>
#include <random>

int main()
{
	hano::Renderer::Info infos = {
		.appName = "Render test 1",
		.windowWidth = 1280,
		.windowHeight = 720
	};

	hano::Renderer renderer(infos);

	hano::Mesh& cubeMesh = renderer.loadMesh("assets/gltf/cube/Cube.gltf");
	hano::Material houseMtrl{ .baseColor = &renderer.loadTexture("assets/textures/back.jpg") };
	hano::Model cube(cubeMesh, houseMtrl);

	auto gltfCube = hano::loadGltfModel(renderer, "assets/gltf/DamagedHelmet.gltf");
	hano::Model simpleCube(gltfCube.mesh, hano::Material{.baseColor = &gltfCube.textures[0]});

	hano::Scene scene;
	scene.camera.pos = glm::vec3(0, 0, -10.0f);
	scene.addModel(simpleCube);
	scene.addModel(cube);
	std::random_device rd{};
	std::mt19937       gen{ rd() };
	std::normal_distribution<float>       xzd{ 1.f, 20.f };
	std::normal_distribution<float>       yd{ 1.f, 20.f };
	std::uniform_real_distribution<float> radd{ .1f, .7f };

	for (int i = 0; i < 100; i++)
	{
		scene.addSphere(hano::Sphere{ glm::vec3(xzd(gen), yd(gen), xzd(gen)), radd(gen) });
	}
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