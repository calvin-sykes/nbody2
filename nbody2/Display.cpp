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
		
		float worldToScreenX(double world_x)
		{
			return static_cast<float>((world_x) / Constants::RADIUS * screen_size.x / (screen_scale * aspect_ratio * 2) + 0.5 * screen_size.x - screen_offset.x);
		}

		float worldToScreenY(double world_y)
		{
			return static_cast<float>(-1 * ((world_y) / Constants::RADIUS) * screen_size.y / (screen_scale * 2) + 0.5 * screen_size.y - screen_offset.y);
		}

		float worldToScreenLength(double world_length)
		{
			return static_cast<float>((world_length) / Constants::RADIUS * screen_size.y / (screen_scale * 2));
		}

		double screenToWorldX(float screen_x)
		{
			return static_cast<float>(((screen_x) - 0.5 * screen_size.x + screen_offset.x) * (2 * aspect_ratio * screen_scale * Constants::RADIUS) / screen_size.x);
		}

		double screenToWorldY(float screen_y)
		{
			return static_cast<float>(-1 * ((screen_y) - 0.5* screen_size.y + screen_offset.y) * (2 * screen_scale * Constants::RADIUS) / screen_size.y);
		}
	}
}