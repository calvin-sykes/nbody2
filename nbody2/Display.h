#ifndef DISPLAY_H
#define DISPLAY_H

#include "Vector.h"

namespace nbody
{
	namespace Display
	{
		float extern aspect_ratio;
		float extern screen_scale;
		Vector2f extern screen_size;
		Vector2f extern screen_offset;

		float extern scaling_cross;
		float extern scaling_smooth;

		float worldToScreenX(double world_x);
		float worldToScreenY(double world_y);
		float worldToScreenLength(double world_length);
		double screenToWorldX(float screen_x);
		double screenToWorldY(float screen_y);
		double screenToWorldLength(float screen_length);

		float bodyScalingFunc(float rad);
	}
}

#endif // DISPLAY_H