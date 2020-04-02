#include <renderer/hanoEditor.hpp>
#include <vulkan/vulkan.hpp>
#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <glm/glm.hpp>
#include <renderer/scene.hpp>
#include <renderer/camera.hpp>

using namespace hano;

namespace {
	void editTransform(Camera const& camera, glm::mat4& matrix)
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
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents((float*)&matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation, 3);
		ImGui::InputFloat3("Rt", matrixRotation, 3);
		ImGui::InputFloat3("Sc", matrixScale, 3);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)&matrix);

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
		ImGui::Checkbox("", &useSnap);
		ImGui::SameLine();

		glm::vec3 snap;
		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap.x);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap.x);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap.x);
			break;
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate((float*)&camera.viewMtr, (float*)&camera.projectionMtr, mCurrentGizmoOperation, mCurrentGizmoMode, (float*)&matrix, NULL, useSnap ? &snap.x : NULL);
	}
}

HanoEditor::HanoEditor(Renderer& renderer)
	: m_renderer(&renderer)
{

}

void HanoEditor::initUI()
{
}

void HanoEditor::drawUI() 
{
	Scene& scene = *m_renderer->getCurrentScene();

	ImGui::ShowMetricsWindow();

	ImGui::Begin("scene");

	if (ImGui::DragFloat3("camPos", (float*)&scene.camera.pos))
	{
		scene.camera.updateViewMtr();
	}

	if (ImGui::DragFloat("fov", &cameraFov))
	{
		scene.camera.setPerspectiveProjection(cameraFov, glm::vec2(m_renderer->getWindowWidth(), m_renderer->getWindowHeight()), 0.01f, 1000.0f);
	}

	for (int i = 0; auto& model : scene.getModels())
	{
		if (ImGui::RadioButton((std::string("model ") + std::to_string(i)).c_str(), selected_index == i))
		{
			selected_index = i;
		}
		i++;
	}

	auto& selectedModel = scene.getModels()[selected_index];

	glm::mat4 model = selectedModel.get().transform.getMatrix();

	editTransform(scene.camera, model);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents((float*)&model, matrixTranslation, matrixRotation, matrixScale);

	selectedModel.get().transform.pos.x = matrixTranslation[0];
	selectedModel.get().transform.pos.y = matrixTranslation[1];
	selectedModel.get().transform.pos.z = matrixTranslation[2];

	//selectedModel.get().transform.rot = glm::quat(glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));

	selectedModel.get().transform.scale.x = matrixScale[0];
	selectedModel.get().transform.scale.y = matrixScale[1];
	selectedModel.get().transform.scale.z = matrixScale[2];

	ImGui::End();
}
