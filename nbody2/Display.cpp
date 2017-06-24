#include "Constants.h"
#include "Display.h"

namespace nbody
{
	namespace Display
	{
		float aspect_ratio = 1;
		float screen_scale = 1;
		Vector2f screen_size = {};
		Vector2f screen_offset = {};

		float scaling_cross = 0.25;
		float scaling_smooth = 0.25;
		double constexpr RADIUS = 1e4 * Constants::PARSEC;


		float worldToScreenX(double world_x)
		{
			return static_cast<float>(world_x / RADIUS * screen_size.x / (screen_scale * aspect_ratio * 2) + 0.5 * screen_size.x - screen_offset.x);
		}

		float worldToScreenY(double world_y)
		{
			return static_cast<float>(-1 * (world_y / RADIUS) * screen_size.y / (screen_scale * 2) + 0.5 * screen_size.y - screen_offset.y);
		}

		float worldToScreenLength(double world_length)
		{
			return static_cast<float>((world_length) / RADIUS * screen_size.y / (screen_scale * 2));
		}

		double screenToWorldX(float screen_x)
		{
			return static_cast<double>(((screen_x)-0.5 * screen_size.x + screen_offset.x) * (2 * aspect_ratio * screen_scale * RADIUS) / screen_size.x);
		}

		double screenToWorldY(float screen_y)
		{
			return static_cast<double>(-1 * ((screen_y)-0.5* screen_size.y + screen_offset.y) * (2 * screen_scale * RADIUS) / screen_size.y);
		}

		float bodyScalingFunc(float rad)
		{
			auto blend = [](auto x, auto cross, auto smooth) { return 0.5 + 0.5 * tanh((x - cross) / smooth); };

			return static_cast<float>(blend(rad, scaling_cross, scaling_smooth) * 0.5 + (1 - blend(rad, scaling_cross, scaling_smooth)) * -log2(rad));
		}
	}
}