#include "Body2d.h"
#include "Constants.h"
//#include "Integrator.h"

namespace nbody
{
	//Integrator * Body2d::integrator_ptr = nullptr;
	size_t Body2d::id_counter = 0;


	Body2d::Body2d(Vector2d const& posIn, Vector2d const& velIn, double const massIn, size_t const idIn)
		: pos(posIn), vel(velIn), acc(), mass(massIn), id(idIn)
	{
	};

	void Body2d::addAccel(Vector2d const& other_pos, double const other_mass)
	{
		auto rel_pos = other_pos - pos; // relative position vector r
		auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
		auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // r/|r|
		// F = (G m1 m2 / (|r|**2 + eps**2) * r_hat
		// a = F / m1
		acc += (Constants::G * other_mass / (rel_pos_mag_sq + Constants::SOFTENING * Constants::SOFTENING)) * unit_vec;
	}

	void Body2d::addAccel(Body2d const& other_body)
	{
		auto rel_pos = other_body.pos - pos; // relative position vector r
		auto rel_pos_mag_sq = rel_pos.mag_sq(); // |r|**2
		auto unit_vec = (1 / sqrt(rel_pos_mag_sq)) * rel_pos; // r/|r|
		// F = (G m1 m2 / (|r|**2 + eps**2) * r_hat
		// a = F / m1
		acc += (Constants::G * other_body.mass / (rel_pos_mag_sq + Constants::SOFTENING * Constants::SOFTENING)) * unit_vec;
	}

	void Body2d::resetAccel()
	{
		acc = {};
	}

	bool Body2d::operator==(Body2d const& other) const
	{
		return id == other.id;
	}

	void Body2d::update(double const dt)
	{
		//integrator_ptr->step(id, pos, vel, acc, dt);
	}
}