#include <renderer/hanoEditor.hpp>
#include <imgui/imgui.h>
#include <renderer/scene.hpp>

using namespace hano;

HanoEditor::HanoEditor(Renderer& renderer)
	: m_renderer(&renderer)
{

}

void HanoEditor::drawUI() 
{
	Scene& scene = *m_renderer->getCurrentScene();

	ImGui::ShowMetricsWindow();

	ImGui::Begin("my window");

	ImGui::DragFloat3("camPos", (float*)&scene.camera.pos);

	if (ImGui::DragFloat("fov", &cameraFov))
	{
		scene.camera.setPerspectiveProjection(cameraFov, glm::vec2(m_renderer->getWindowWidth(), m_renderer->getWindowWidth()), 0.01f, 1000.0f);
	}

#if 0
	for (int i = 0; auto & mesh : m_currentScene->meshes)
	{
		ImGui::DragFloat3((std::string("pos ") + std::to_string(i)).c_str(), (float*)&mesh.transform.pos);
		ImGui::DragFloat4((std::string("rot ") + std::to_string(i)).c_str(), (float*)&mesh.transform.rot);
		ImGui::DragFloat3((std::string("scale ") + std::to_string(i)).c_str(), (float*)&mesh.transform.scale);

		glm::mat4 model = mesh.transform.getMatrix();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate((float*)&scene.camera.viewMtr,
			(float*)&scene.camera.projectionMtr, ImGuizmo::TRANSLATE,
			ImGuizmo::WORLD, (float*)&model, NULL, NULL);

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		ImGuizmo::DecomposeMatrixToComponents((float*)&model, matrixTranslation, matrixRotation, matrixScale);

		mesh.transform.pos.x = matrixTranslation[0];
		mesh.transform.pos.y = matrixTranslation[1];
		mesh.transform.pos.z = matrixTranslation[2];

		i++;
	}
#endif
	ImGui::End();
}
