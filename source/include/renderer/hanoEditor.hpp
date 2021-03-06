#pragma once

#include "editorGUI.hpp"
#include "hanoRenderer.hpp"

namespace hano
{
	class HanoEditor : public CustomEditorGUI
	{
		public:

			HanoEditor(Renderer& renderer);
			virtual void initUI() override;
			virtual void drawUI() override;

		private:

			Renderer* m_renderer;
	};
}