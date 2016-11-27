#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <array>

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
	union
	{
		struct
		{
			T x, y;
		};
		impl_type a_;
	};

	Components() = default;
	Components(T const val) : a_{ val } {};
	Components(T const x, T const y) : a_{ x, y } {};
};

template <typename T>
struct Components<T, 3>
{
	using impl_type = std::array<T, 3>;
	union
	{
		struct
		{
			T x, y, z;
		};
		impl_type a_;
	};

	Components() = default;
	Components(T const val) : a_{ val } {};
	Components(T const x, T const y, T const z) : a_{ x, y, z } {};
};

#endif // COMPONENTS_H
