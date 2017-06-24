#include "ComplexCDF.h"
#include "Error.h"
#include <cassert>

namespace nbody
{
	ComplexCDF::ComplexCDF() :
		m_min(0),
		m_max(0),
		m_width(0),
		//m_n_steps(0),
		m_i0(0),
		m_k(0),
		m_a(0),
		m_r_bulge(0)
	{
	}

	ComplexCDF::~ComplexCDF()
	{
	}
	void ComplexCDF::setup(double const i0, double const k, double const a, double const rad_bulge,
		double const min, double const max, size_t const n_steps)
	{
		m_min = min;
		m_max = max;
		//m_n_steps = n_steps;

		m_i0 = i0;
		m_k = k;
		m_a = a;
		m_r_bulge = rad_bulge;

		//build(m_n_steps);
		build(n_steps);
	}

	double ComplexCDF::valFromProbability(double p) const
	{
		assert(p >= 0 && p <= 1);

		auto h = 1.0 / (m_y2.size() - 1);

		auto i = static_cast<size_t>(p / h);
		auto remainder = p - i * h;

		assert(i >= 0 && i < m_m2.size());
		return m_y2[i] + m_m2[i] * remainder;
	}

	void ComplexCDF::build(size_t const n_steps)
	{
		auto h = (m_max - m_min) / n_steps;
		auto x = 0.0, y = 0.0;

		m_y1.push_back(0.0);
		m_x1.push_back(0.0);

		// Simpson's rule to integrate distribution function
		for (auto i = 0; i < n_steps; i += 2)
		{
			x = (i + 2) * h;
			y += h / 3 * (intensity(m_min + i * h)
				+ 4 * intensity(m_min + (i + 1) * h)
				+ intensity(m_min + (i + 2) * h));

			m_m1.push_back((y - m_y1.back()) / (2 * h));
			m_y1.push_back(y);
			m_x1.push_back(x);
		}
		m_m1.push_back(0.0);

		if (m_m1.size() != m_x1.size() || m_m1.size() != m_y1.size())
			throw MAKE_ERROR("ComplexCDF array size msimatch (1)");

		// normalise
		for (auto i = 0; i < m_y1.size(); i++)
		{
			m_m1[i] /= m_y1.back();
			m_y1[i] /= m_y1.back();
		}

		m_y2.push_back(0.0);
		m_x2.push_back(0.0);

		auto p = 0.0;
		h = 1.0 / n_steps;
		for (auto i = 1; i < n_steps; i++)
		{
			auto k = 0;
			p = i * h;

			while (m_y1[k + 1] <= p)
				k++;

			y = m_x1[k] + (p - m_y1[k]) / m_m1[k];

			m_m2.push_back((y - m_y2.back()) / h);
			m_y2.push_back(y);
			m_x2.push_back(p);
		}
		m_m2.push_back(0.0);

		if (m_m2.size() != m_x2.size() || m_m2.size() != m_y2.size())
			throw MAKE_ERROR("ComplexCDF array size msimatch (2)");
	}

	double ComplexCDF::intensity(double const x) const
	{
		return (x < m_r_bulge) ?
			intensityBulge(x, m_i0, m_k) :
			intensityDisc(x - m_r_bulge, intensityBulge(m_r_bulge, m_i0, m_k), m_a);
	}

	double ComplexCDF::intensityBulge(double const r, double const i0, double const k) const
	{
		return i0 * exp(-k * pow(r, 0.25));
	}

	double ComplexCDF::intensityDisc(double const r, double const i0, double const a) const
	{
		return i0 * exp(-r / a);
	}
}
