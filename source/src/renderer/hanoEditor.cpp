#include <renderer/hanoEditor.hpp>
#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <renderer/scene.hpp>
#include <renderer/camera.hpp>
#include <renderer/UILogger.hpp> 
#include <fmt/format.h>

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
			transform.rot = glm::quat(glm::radians(transform.eulerRot));
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

		static glm::vec3 snap;
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
			glm::mat4 gizmoProj = camera.projectionMtr;
			gizmoProj[1][1] *= -1; // imguizmo uses openGL coordSystem
			ImGuizmo::Manipulate(glm::value_ptr(camera.viewMtr), glm::value_ptr(gizmoProj), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(modelMatrix), glm::value_ptr(deltaMatrix), useSnap ? &snap.x : NULL);
			
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), glm::value_ptr(transform.pos), glm::value_ptr(transform.eulerRot), glm::value_ptr(transform.scale));
			transform.rot = glm::quat(glm::radians(transform.eulerRot));
		}
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

	UILogger& logger = UILogger::Instance();

	static bool show_demo = true;
	static bool show_logger = true;

	if (show_demo)
		ImGui::ShowDemoWindow(&show_demo);
	if (show_logger)
		logger.draw("Logger", &show_logger);

	if (ImGui::Button("testLog"))
	{
		hano_info("salut la {}", "compagnie");
		hano_warn("warn test {}", 45);
		hano_error("error test {}", 45.2);
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("demo", nullptr, &show_demo);
			ImGui::MenuItem("logger", nullptr, &show_logger);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::Begin("scene");

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
	ImGui::Text("Application average %.4f ms/frame (%.2f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
	ImGui::Text("%d active windows (%d visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
	ImGui::Text("%d active allocations", io.MetricsActiveAllocations);

	if (ImGui::Button("reload shaders"))
	{
		m_renderer->reloadShaders();
	}

	if (ImGui::DragFloat3("camPos", (float*)&scene.camera.pos, 0.2f))
	{
		scene.camera.updateViewMtr();
	}

	static float cameraFov = 45.0f;
	if (ImGui::DragFloat("fov", &cameraFov))
	{
		scene.camera.setPerspectiveProjection(cameraFov, glm::vec2(m_renderer->getWindowWidth(), m_renderer->getWindowHeight()), 0.01f, 1000.0f);
	}

	ImGui::Text("models :");
	static int selected_index = 0;
	for (int i = 0; auto & model : scene.getModels())
	{
		if (ImGui::RadioButton(fmt::format("model {}", i).c_str(), selected_index == i))
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
			scene.lights.push_back(PointLight{ .intensity = 1.0f, .color = glm::vec3(1.0f, 1.0f, 1.0f) });
			hano_info("added point light");
		}
		ImGui::SameLine();
		if (ImGui::Button("RemoveLight"))
		{
			scene.lights.pop_back();
			hano_info("removed point light");
		}

		for (int i = 0; auto & light : scene.lights)
		{
			ImGui::DragFloat3(fmt::format("light pos {}", i).c_str(), (float*)&light.pos);
			ImGui::ColorEdit3(fmt::format("light color {}", i).c_str(), (float*)&light.color);
			ImGui::DragFloat(fmt::format("light intensity {}", i).c_str(), (float*)&light.intensity, 0.01f, 0.0f, 1.0f);
			ImGui::Text("\n");
			i++;
		}
	}
	
	ImGui::End();
}
