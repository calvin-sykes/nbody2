#include "IDistributor.h"
#include "Constants.h"
#include "Vector.h"

namespace nbody
{
	namespace priv
	{
		Vector2d vCirc(Vector2d const & r, double const central_mass)
		{
			auto mag_v = sqrt(Constants::G * central_mass / r.mag());
			auto angle_v = atan(abs(r.y / r.x));
			auto v_x = -1 * sgn(r.y) * sin(angle_v) * mag_v;
			auto v_y = sgn(r.x) * cos(angle_v) * mag_v;
			return{ v_x, v_y };
		}
	}

	std::random_device dev{};
	std::default_random_engine IDistributor::m_gen{ dev() };
	std::uniform_real_distribution<> IDistributor::m_dist{ 0, 1 };
}