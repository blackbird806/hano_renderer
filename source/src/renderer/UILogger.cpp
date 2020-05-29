#include <renderer/UILogger.hpp>

using namespace hano;

UILogger& UILogger::Instance()
{
	static UILogger inst;
	return inst;
}

void UILogger::clear() 
{ 
	buf.clear();
	lineColors.clear();
	lineOffsets.clear(); 
}

void UILogger::draw(const char* title, bool* p_opened)
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin(title, p_opened);
	if (ImGui::Button("Clear")) clear();
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	filter.Draw("Filter", -100.0f);
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	if (copy) ImGui::LogToClipboard();

	const char* buf_begin = buf.begin();
	const char* line = buf_begin;

	if (!buf.empty())
	{
		for (int line_no = 0; line != NULL; line_no++)
		{
			const char* line_end = (line_no < lineOffsets.Size) ? buf_begin + lineOffsets[line_no] : NULL;

			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)lineColors[line_no]);

			if (filter.IsActive())
			{
				if (filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
			}
			else
			{
				ImGui::TextUnformatted(line, line_end);
			}

			ImGui::PopStyleColor();

			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}
	}

	if (scrollToBottom)
		ImGui::SetScrollHere(1.0f);
	scrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}