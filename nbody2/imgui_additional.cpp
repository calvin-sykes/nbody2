#include "Config.h"
#include "imgui_additional.h"

#include <stdarg.h>

namespace ImGui
{
	void ShowHelpMarker(char const* fmt, ...)
	{
		TextDisabled("(?)");
		if (IsItemHovered())
		{
			BeginTooltip();
			PushTextWrapPos(200.0f);
			va_list args;
			va_start(args, fmt);
			TextWrappedV(fmt, args);
			va_end(args);
			PopTextWrapPos();
			EndTooltip();
		}
	}

	bool CentredButton(char const* label, ImVec2 const& size)
	{
		size_t n_lines = 0;
		auto len = strlen(label);
		auto lines = new char*[len + 1]; // Worst case: every character is carriage return

		// Make a modifiable copy of the argument
		auto label_cpy = new char[len + 1];
		// Split the argument acroos newline characters
		char * next_tok = nullptr;
		strncpy(label_cpy, label, len + 1);
#ifdef SAFE_STRFN
		auto tok = strtok_s(label_cpy, "\n", &next_tok);
		while (tok != nullptr)
		{
			lines[n_lines] = new char[strlen(tok) + 1];
			strncpy(lines[n_lines++], tok, strlen(tok) + 1);
			tok = strtok_s(nullptr, "\n", &next_tok);
		}
#else
		auto tok = strtok(label_cpy, "\n");
		while (tok != nullptr)
		{
			lines[n_lines] = new char[strlen(tok) + 1];
			strncpy(lines[n_lines++], tok, strlen(tok) + 1);
			tok = strtok(nullptr, "\n");
		}
#endif

		// Figure out length of each line and find the maximum length
		size_t* line_lens = new size_t[n_lines];
		size_t max_len = 0;
		for (size_t i = 0; i < n_lines; i++)
		{
			line_lens[i] = strlen(lines[i]);
			if (line_lens[i] > max_len)
				max_len = line_lens[i];
		}

		// Length of formatted string
		//	= number of lines * ( longest line * + 1 for newlines / null terminator)
		auto formatted_len = (max_len + 1) * n_lines;
		char* formatted = new char[formatted_len];
		// copy strings, padding with spaces either side
		for (size_t i = 0; i < n_lines; i++)
		{
			size_t pos = i * (max_len + 1);
			// num of chars to fill with spaces
			auto extras = max_len - line_lens[i];
			// before text
			memset(static_cast<void*>(formatted + pos), ' ', extras / 2);
			pos += extras / 2;
			// text
#ifdef SAFE_STRFN
			strncpy_s(formatted + pos, formatted_len - pos, lines[i], line_lens[i]);
#else
			strncpy(formatted + pos, lines[i], line_lens[i]);
#endif		
			pos += line_lens[i];
			// after text until line is full, overwriting newline / null terminator
			auto remaining_len = max_len - (pos - (max_len + 1) * i);
			memset(static_cast<void*>(formatted + pos), ' ', remaining_len);
			pos += remaining_len;
			// replace newline / null terminator
			formatted[pos] = (i != n_lines - 1) ? '\n' : '\0';
		}

		// clean up
		delete[] label_cpy;
		for (size_t i = 0; i < n_lines; i++)
		{
			delete[] lines[i];
		};
		delete[] line_lens;
		delete[] lines;

		// display button before deleting the formatted label
		auto res = Button(formatted, size);
		delete[] formatted;
		return res;
	}

	bool ColorEdit3_sf(const char * label, sf::Color * col)
	{
		float decimal_cols[3] = {col->r / 255.f, col->g / 255.f, col->b / 255.f };
		const auto value_changed = ColorEdit3(label, decimal_cols);
		*col = sf::Color{ static_cast<sf::Uint8>(decimal_cols[0] * 255),
			static_cast<sf::Uint8>(decimal_cols[1] * 255),
			static_cast<sf::Uint8>(decimal_cols[2] * 255) };
		return value_changed;
	}
}