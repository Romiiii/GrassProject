#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include <imgui.h>

/**
 * \brief Draws the tooltip
*/
inline void drawTooltip(const char* desc)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


namespace ImGui
{

}

#endif // !GUI_HELPERS_H
