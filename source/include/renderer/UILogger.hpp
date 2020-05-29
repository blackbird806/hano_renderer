#pragma once
#include <string_view>
#include <fmt/format.h>
#include <imgui/imgui.h>

#define HANO_LOG_INFO_COLOR ImColor(0.0f, 1.0f, 0.0f)
#define HANO_LOG_WARN_COLOR ImColor(1.0f, 0.5f, 0.0f)
#define HANO_LOG_ERROR_COLOR ImColor(1.0f, 0.0f, 0.0f)

namespace hano
{
	class UILogger
	{
		ImGuiTextBuffer     buf;
		ImGuiTextFilter     filter;
		ImVector<int>       lineOffsets;
		ImVector<ImColor>	lineColors;
		bool                scrollToBottom;

	public:

		static UILogger& Instance();

		void clear();

		template<typename ...Args>
		void addLog(ImColor col, std::string_view fmt, Args ...args)
		{
			int old_size = buf.size();
			buf.append(fmt::format(fmt, args...).c_str());
			buf.append("\n");
			for (int new_size = buf.size(); old_size < new_size; old_size++)
				if (buf[old_size] == '\n')
					lineOffsets.push_back(old_size);
			scrollToBottom = true;
			lineColors.push_back(col);
		}

		void draw(const char* title, bool* p_opened = nullptr);
	};
}