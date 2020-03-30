#pragma once

#include <memory>
#include <filesystem>
#include <renderer/editorGUI.hpp>
#include <renderer/scene.hpp>
#include <renderer/resourceManager.hpp>
#include "vulkanContext.hpp"

struct GLFWwindow;

namespace hano
{
	class Renderer
	{
		public:

			struct Info
			{
				const char* appName;
				int windowWidth, windowHeight;
			};

			Renderer();
			Renderer(Info const& infos);
			void init(Info const& infos);

			void destroy();

			~Renderer();

			Mesh& loadMesh(std::filesystem::path const& meshPath);
			Texture& loadTexture(std::filesystem::path const& texturePath);

			void setRenderScene(Scene& scene);
			void setEditorGUI(CustomEditorGUI& editor);
			void renderFrame();

			bool isRunning() const noexcept;

			int getWindowWidth() const noexcept;
			int getWindowHeight() const noexcept;

			Scene* getCurrentScene();

		private:

			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

			VulkanContext m_vkContext;
			ResourceManager m_resourceMgr;
			GLFWwindow* m_window;
			int m_windowWidth;
			int m_windowHeight;

			std::unique_ptr<EditorGUI> m_editorGUI;
			Scene* m_currentScene;
			bool m_isRunning;
	};
}
