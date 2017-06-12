#ifndef IMGUI_ADDITIONAL_H
#define IMGUI_ADDITIONAL_H

#include "imgui.h"

namespace ImGui
{	
	void ShowHelpMarker(char const* fmt, ...);

	bool CentredButton(char const* label, ImVec2 const& size);

	bool ColorEdit3_sf(const char* label, sf::Color * col);
}

#endif // IMGUI_ADDITIONAL_H