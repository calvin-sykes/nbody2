#ifndef COMPLEX_CDF_H
#define COMPLEX_CDF_H

#include <cstddef> // for size_t under GCC
#include <vector>

namespace nbody
{
	class ComplexCDF
	{
	public:
		ComplexCDF();
		~ComplexCDF();

		void setup(double const i0, double const k, double const a, double const rad_bulge,
			double const min, double const max, size_t const n_steps);

		double valFromProbability(double p) const;

	private:
		double m_min, m_max, m_width;
		//size_t m_n_steps;
		
		double m_i0, m_k, m_a, m_r_bulge;

		std::vector<double> m_m1;
		std::vector<double> m_y1;
		std::vector<double> m_x1;

		std::vector<double> m_m2;
		std::vector<double> m_y2;
		std::vector<double> m_x2;

		void build(size_t const n_steps);

		double intensity(double x) const;
		double intensityBulge(double const r, double const i0, double const k) const;
		double intensityDisc(double const r, double const i0, double  const a) const;

	};
}

#endif // COMPLEX_CDF_H
