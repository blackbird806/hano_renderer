#include <renderer/hanoEditor.hpp>
#include <vulkan/vulkan.hpp>
#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <renderer/scene.hpp>
#include <renderer/camera.hpp>

#include <thread>
#include <chrono>

using namespace hano;

namespace {

	void editTransform(Camera const& camera, Transform& transform)
	{
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		glm::vec3 skew;
		glm::vec4 perspective;
		ImGui::DragFloat3("pos", glm::value_ptr(transform.pos));

		if (ImGui::DragFloat3("rot", glm::value_ptr(transform.eulerRot)))
		{
		}

		ImGui::DragFloat3("scale", glm::value_ptr(transform.scale));
		glm::mat4 modelMatrix;
		ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(transform.pos), glm::value_ptr(transform.eulerRot), glm::value_ptr(transform.scale), glm::value_ptr(modelMatrix));

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		static bool useSnap(false);
		if (ImGui::IsKeyPressed(83))
			useSnap = !useSnap;
		ImGui::Checkbox("snap ", &useSnap);
		ImGui::SameLine();

		glm::vec3 snap;
		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::DragFloat3("Snap", &snap.x);
			break;
		case ImGuizmo::ROTATE:
			ImGui::DragFloat3("Angle Snap", &snap.x);
			break;
		case ImGuizmo::SCALE:
			ImGui::DragFloat3("Scale Snap", &snap.x);
			break;
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		
		static bool show_gizmos = true;
		ImGui::Checkbox("show transform gizmo", &show_gizmos);
		if (show_gizmos)
		{
			glm::mat4 deltaMatrix;
			ImGuizmo::Manipulate(glm::value_ptr(camera.viewMtr), glm::value_ptr(camera.projectionMtr), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(modelMatrix), glm::value_ptr(deltaMatrix), useSnap ? &snap.x : NULL);
			
			glm::quat rotation;
			glm::decompose(modelMatrix, transform.scale, rotation, transform.pos, skew, perspective);
			transform.rot = glm::conjugate(rotation);
		}
		transform.eulerRot = glm::eulerAngles(transform.rot);
	}
}

HanoEditor::HanoEditor(Renderer& renderer)
	: m_renderer(&renderer)
{

}

void HanoEditor::initUI()
{
	m_renderer->reloadShaders();
}

void HanoEditor::drawUI()
{
	Scene& scene = *m_renderer->getCurrentScene();
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	ImGui::ShowDemoWindow();

	ImGui::Begin("scene");

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
	ImGui::Text("Application average %.4f ms/frame (%.2f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
	ImGui::Text("%d active windows (%d visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
	ImGui::Text("%d active allocations", io.MetricsActiveAllocations);

	static float maxFps = 30.0f;
	ImGui::DragFloat("max fps", &maxFps, 1.0f, 0.0f, 144.0f);



	if (ImGui::Button("reload shaders"))
	{
		m_renderer->reloadShaders();
	}

	if (ImGui::DragFloat3("camPos", (float*)&scene.camera.pos))
	{
		scene.camera.updateViewMtr();
	}

	static float cameraFov = 45.0f;
	if (ImGui::DragFloat("fov", &cameraFov))
	{
		scene.camera.setPerspectiveProjection(cameraFov, glm::vec2(m_renderer->getWindowWidth(), m_renderer->getWindowHeight()), 0.01f, 1000.0f);
	}

	static int selected_index = 0;
	for (int i = 0; auto & model : scene.getModels())
	{
		if (ImGui::RadioButton((std::string("model ") + std::to_string(i)).c_str(), selected_index == i))
		{
			selected_index = i;
		}
		i++;
	}

	auto& selectedModel = scene.getModels()[selected_index].get();
	editTransform(scene.camera, selectedModel.transform);

	if (ImGui::CollapsingHeader("Lights"))
	{
		if (ImGui::Button("AddLight"))
		{
			scene.lights.push_back(PointLight{});
		}
		ImGui::SameLine();
		if (ImGui::Button("RemoveLight"))
		{
			scene.lights.pop_back();
		}
		ImGui::SameLine();
		static bool showLightGizmo = true;
		ImGui::Checkbox("show light gizmo", &showLightGizmo);

		for (int i = 0; auto & light : scene.lights)
		{
			ImGui::DragFloat3((std::string("light pos") + std::to_string(i)).c_str(), (float*)&light.pos);
			ImGui::DragFloat3((std::string("light color") + std::to_string(i)).c_str(), (float*)&light.color, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat((std::string("light intensity") + std::to_string(i)).c_str(), (float*)&light.intensity, 0.01f, 0.0f, 1.0f);

			//drawList->AddCircleFilled({ screenPos.x, screenPos.y }, 50.0, 0xFFFFFFFF, 30);

			i++;
		}
	}

	ImGui::End();
}
