#ifndef VECTOR_H
#define VECTOR_H

#include "Components.h"

#include <SFML\System\Vector2.hpp>
#include <SFML\System\Vector3.hpp>

namespace nbody
{
	template <typename T, size_t N>
	class Vector : public Components<T, N>
	{
		// import constructors of underlying storage
		using Components<T, N>::Components;

		// Prevent instantiation of vectors of non-numerical types, for which arithmetic operators are not defined
		static_assert(std::is_arithmetic<T>::value, "Vector element type must be a numerical type.");

	public:

		//////////////////
		// Constructors //
		//////////////////

		// Construct zero vector
		Vector() noexcept : Components<T, N>() {};

		// Construct vector with all values equal to val
		Vector(T const val) noexcept : Components<T, N>(val) {};

		// Construct from an SFML vector (2D)
		template <typename Tin, int Dim = N, typename Sfinae = typename std::enable_if<(Dim == 2)>::type>
		Vector(sf::Vector2<Tin> const& source) :
			Components<T, 2>(static_cast<T>(source.x), static_cast<T>(source.y)) {};

		// Construct from an SFML vector (3D)
		template <typename Tin, int Dim = N, typename Sfinae = typename std::enable_if<(Dim == 3)>::type>
		Vector(sf::Vector3<Tin> const& source) :
			Components<T, 3>( static_cast<T>(source.x), static_cast<T>(source.y), static_cast<T>(source.z)) {};

		// Copy-construct a vector
		Vector(Vector const& source) noexcept = default;

		// Move-construct a vector
		Vector(Vector && source) noexcept = default;

		// Destructor
		~Vector() noexcept = default;

		// Conversion to SFML vector (2D)
		template <typename Tout, int Dim = N, typename Sfinae = typename std::enable_if<(Dim == 2)>::type>
		operator sf::Vector2<Tout>()
		{
			return { static_cast<Tout>(a_[0]), static_cast<Tout>(a_[1]) };
		}

		// Conversion to SFML vector (3D)
		template <typename Tout, int Dim = N, typename Sfinae = typename std::enable_if<(Dim == 3)>::type>
		operator sf::Vector3<Tout>()
		{
			return { static_cast<Tout>(a_[0]), static_cast<Tout>(a_[1]), static_cast<Tout>(a_[2]) };
		}

		///////////////////////////
		// Arithmetic operations //
		///////////////////////////

		// Addition
		Vector& operator+=(const Vector& rhs)
		{
			for (size_t i = 0; i < N; i++)
				a_[i] += rhs.a_[i];
			return *this;
		}

		// Subtraction
		Vector& operator-=(const Vector& rhs)
		{
			for (size_t i = 0; i < N; i++)
				a_[i] -= rhs.a_[i];
			return *this;
		}

		// Scalar multiplication
		Vector& operator*=(const T factor)
		{
			for (size_t i = 0; i < N; i++)
				a_[i] *= factor;
			return *this;
		}

		// Dot product
		T dot_prod(const Vector& rhs) const
		{
			T dp = 0;
			for (size_t i = 0; i < N; i++)
				dp += a_[i] * rhs.a_[i];
			return dp;
		}

		// Magnitude
		T mag() const
		{
			return sqrt(mag_sq());
		}

		// Magnitude squared
		T mag_sq() const
		{
			return dot_prod(*this);
		}

		///////////////////////////
		// Assignment operations //
		///////////////////////////

		// Copy assignment
		Vector& operator=(Vector const& other)
		{
			// copy
			Vector tmp(other);
			// swap
			std::swap(a_, tmp.a_);
			return *this;
		}

		// Move assignment
		Vector& operator=(Vector&& other)
		{
			a_ = std::move(other.a_);
			return *this;
		}

		// Test for equality
		bool operator==(const Vector& rhs) const
		{
			bool equal = true;
			for (size_t i = 0; i < N; i++)
			{
				equal &= fabs(a_[i] - rhs.a_[i]) <
					std::numeric_limits<T>().epsilon() * max(fabs(a_[i]), fabs(rhs.a_[i]));
			}
			return equal;
		}

		// Test for inequality 
		bool operator!=(const Vector& rhs) const
		{
			return !operator==(rhs);
		}

		// Element access
		T const& operator[](size_t index)
		{
			return a_[i];
		}
	};

	template <typename T, size_t N>
	std::ostream& operator<<(std::ostream& os, const Vector<T, N>& v)
	{
		for (size_t i = 0; i < N; i++)
			os << v[i] << ' ';
		return os;
	}

	template <typename T, size_t N>
	Vector<T, N> operator+(Vector<T, N> lhs, const Vector<T, N>& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	template <typename T, size_t N>
	Vector<T, N> operator-(Vector<T, N> lhs, const Vector<T, N>& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	template <typename T, typename F, size_t N, typename std::enable_if<std::is_arithmetic<F>::value>::type* = nullptr>
	Vector<T, N> operator*(Vector<T, N> v, const F f)
	{
		v *= static_cast<T>(f);
		return v;
	}

	template <typename T, typename F, size_t N, typename std::enable_if<std::is_arithmetic<F>::value>::type* = nullptr>
	Vector<T, N> operator*(const F f, Vector<T, N> v)
	{
		return v *= static_cast<T>(f);
		return v;
	}

	typedef Vector<float, 2> Vector2f;
	typedef Vector<float, 3> Vector3f;

	typedef Vector<double, 2> Vector2d;
	typedef Vector<double, 3> Vector3d;
}

#endif // VECTOR_H