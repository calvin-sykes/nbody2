#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <array>

namespace nbody
{
	namespace priv
	{
		template <typename T, size_t N>
		struct Components
		{
			using impl_type = std::array<T, N>;
			impl_type a_;
		};

		template <typename T>
		struct Components<T, 2>
		{
			using impl_type = std::array<T, 2>;
			union alignas(16)
			{
				struct
				{
					T x, y;
				};
				impl_type a_;
			};

			Components() : a_{0.0, 0.0} {};
			Components(T const val) : a_{ val, val } {};
			Components(T const x, T const y) : a_{ x, y } {};
		};

		template <typename T>
		struct Components<T, 3>
		{
			using impl_type = std::array<T, 3>;
			union alignas(32)
			{
				struct
				{
					T x, y, z;
				};
				impl_type a_;
			};

			Components() : a_{0.0, 0.0} {};
			Components(T const val) : a_{ val, val, val } {};
			Components(T const x, T const y, T const z) : a_{ x, y, z } {};
		};
	}
}

#endif // COMPONENTS_H
