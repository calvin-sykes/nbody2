#ifndef IMGUI_ADDITIONAL_H
#define IMGUI_ADDITIONAL_H

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{	
	void ShowHelpMarker(char const* fmt, ...);

	IMGUI_API bool CentredButton(char const* label, ImVec2 const& size);

	IMGUI_API bool ColorEdit3_sf(const char* label, sf::Color * col);
}

#endif // IMGUI_ADDITIONAL_H