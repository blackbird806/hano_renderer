#include <hanoRenderer.hpp>
#include <renderer/hanoEditor.hpp>
#include <renderer/gltfLoader.hpp>
#include <random>
#include <chrono>
#include <thread>

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

	//auto gltfModel = hano::loadGltfModel(renderer, "assets/gltf/DamagedHelmet.gltf");
	hano::Mesh& spawnMesh = renderer.loadMesh("assets/obj/house.obj");
	hano::Material spawnMtrl{ .baseColor = &renderer.loadTexture("assets/textures/house-RGBA.png") };
	hano::Model simpleCube(spawnMesh, spawnMtrl);

	hano::Scene scene;
	scene.camera.pos = glm::vec3(0, 0, -10.0f);
	scene.addModel(simpleCube);
	scene.addModel(cube);
	std::random_device rd{};
	std::mt19937       gen{ rd() };
	std::normal_distribution<float>       xzd{ 1.f, 10.f };
	std::normal_distribution<float>       yd{ 1.f, 10.f };
	std::uniform_real_distribution<float> radd{ .1f, .7f };

	for (int i = 0; i < 10; i++)
	{
		scene.addSphere(hano::Sphere{ glm::vec3(xzd(gen), yd(gen), xzd(gen)), radd(gen) });
	}
	//simpleCube.transform.pos = glm::vec3(1, 0, 2);
	cube.transform.pos = glm::vec3(-1, 0, 2);

	scene.addLight(hano::PointLight{ 1.0f, glm::vec3(2.0f, 1.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f) });

	renderer.setRenderScene(scene);

	hano::HanoEditor editor(renderer);
	renderer.setEditorGUI(editor);

	auto last = std::chrono::steady_clock::now();

	while (renderer.isRunning())
	{
		using namespace std::chrono_literals;

		auto before = std::chrono::steady_clock::now();
		renderer.renderFrame();
		auto after = std::chrono::steady_clock::now();
		auto deltaTime = after - before;
		std::chrono::duration<double, std::milli> deltaMs = deltaTime;
		std::chrono::duration<double, std::milli> const targetFrameTime = 16.66ms;

		if (deltaMs < targetFrameTime)
		{
			std::this_thread::sleep_for(targetFrameTime - deltaMs);
		}
	}

	renderer.waitIdle();

    return 0;
}