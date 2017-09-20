#ifndef VECTOR_H
#define VECTOR_H

#include "Components.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include "Config.h"

#include <emmintrin.h>
#include <cmath>
#include <limits>

namespace nbody
{
	template <typename T, size_t N>
	class Vector : public priv::Components<T, N>
	{
		// import constructors of underlying storage
		using priv::Components<T, N>::Components;
		using priv::Components<T, N>::a_;

		// Prevent instantiation of vectors of non-numerical types, for which arithmetic operators are not defined
		static_assert(std::is_arithmetic<T>::value, "Vector element type must be a numerical type.");

	public:

		//////////////////
		// Constructors //
		//////////////////

		// Construct zero vector
		Vector() noexcept : priv::Components<T, N>() {}

		// Construct vector with all values equal to val
		explicit Vector(T const val) noexcept : priv::Components<T, N>(val) {}

		// Construct from an SFML vector (2D)
		template <typename Tin, int Dim = N, std::enable_if_t<(Dim == 2)>* = nullptr>
		explicit Vector(sf::Vector2<Tin> const& source) :
			priv::Components<T, 2>(static_cast<T>(source.x), static_cast<T>(source.y)) {}

		// Construct from an SFML vector (3D)
		template <typename Tin, int Dim = N, std::enable_if_t<(Dim == 3)>* = nullptr>
		explicit Vector(sf::Vector3<Tin> const& source) :
			priv::Components<T, 3>( static_cast<T>(source.x), static_cast<T>(source.y), static_cast<T>(source.z)) {}

		// Construct from an SSE type (2D)
		template <int Dim = N, std::enable_if_t<(Dim == 2)>* = nullptr>
		explicit Vector(__m128d const& v) :
			priv::Components<T, 2>(static_cast<T>(SSE_ACCESS(v,m128d_f64,0)), static_cast<T>(SSE_ACCESS(v,m128d_f64,1))) {}
            //priv::Components<T, 2>(static_cast<T>(v.m128d_f64[0]), static_cast<T>(v.m128d_f64[1])) {}

		// Copy-construct a vector
		Vector(Vector const& source) noexcept = default;

		// Move-construct a vector
		Vector(Vector && source) noexcept = default;

		// Destructor
		~Vector() noexcept = default;

		// Conversion to SFML vector (2D)
		template <typename Tout, int Dim = N, std::enable_if_t<(Dim == 2)>* = nullptr>
		operator sf::Vector2<Tout>()
		{
			return { static_cast<Tout>(a_[0]), static_cast<Tout>(a_[1]) };
		}

		// Conversion to SFML vector (3D)
		template <typename Tout, int Dim = N, std::enable_if_t<(Dim == 3)>* = nullptr>
		operator sf::Vector3<Tout>()
		{
			return { static_cast<Tout>(a_[0]), static_cast<Tout>(a_[1]), static_cast<Tout>(a_[2]) };
		}

		///////////////////////////
		// Arithmetic operations //
		///////////////////////////

		// Addition
		Vector& operator+=(Vector const& rhs)
		{
			for (size_t i = 0; i < N; i++)
				a_[i] += rhs.a_[i];
			return *this;
		}

		// Subtraction
		Vector& operator-=(Vector const& rhs)
		{
			for (size_t i = 0; i < N; i++)
				a_[i] -= rhs.a_[i];
			return *this;
		}

		// Scalar multiplication
		Vector& operator*=(T const factor)
		{
			for (size_t i = 0; i < N; i++)
				a_[i] *= factor;
			return *this;
		}

		//Scalar division
		Vector& operator/=(T const factor)
		{
			return operator*=(1 / factor);
		}

		// Dot product
		T dot_prod(Vector const& rhs) const
		{
			T dp = 0;
			for (size_t i = 0; i < N; i++)
				dp += a_[i] * rhs.a_[i];
			return dp;
		}

		// Magnitude
		T mag() const
		{
			return std::sqrt(mag_sq());
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
		Vector& operator=(Vector&& other) noexcept
		{
			a_ = std::move(other.a_);
			return *this;
		}

		// Test for equality
		bool operator==(Vector const& rhs) const
		{
			auto equal = true;
			for (size_t i = 0; i < N && equal; i++)
			{
				//equal &= a_[i] == rhs.a_[i];
				equal &= std::abs(a_[i] - rhs.a_[i]) <=
					std::numeric_limits<T>().epsilon() * std::max(std::abs(a_[i]), std::abs(rhs.a_[i]));
			}
			return equal;
		}

		// Test for inequality 
		bool operator!=(Vector const& rhs) const
		{
			return !operator==(rhs);
		}

		// Element access
		T operator[](size_t index) const
		{
			return a_[index];
		}
	};

	template <typename T, size_t N>
	std::ostream& operator<<(std::ostream& os, Vector<T, N> const& v)
	{
		for (size_t i = 0; i < N; i++)
			os << v[i] << ' ';
		return os;
	}

	template <typename T, size_t N>
	Vector<T, N> operator+(Vector<T, N> lhs, Vector<T, N> const& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	template <typename T, size_t N>
	Vector<T, N> operator-(Vector<T, N> lhs, Vector<T, N> const& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	template <typename T, typename F, size_t N, std::enable_if_t<std::is_convertible<F, T>::value>* = nullptr>
	Vector<T, N> operator*(Vector<T, N> v, F const f)
	{
		v *= static_cast<T>(f);
		return v;
	}

	template <typename T, typename F, size_t N, std::enable_if_t<std::is_convertible<F, T>::value>* = nullptr>
	Vector<T, N> operator*(F const f, Vector<T, N> v)
	{
		v *= static_cast<T>(f);
		return v;
	}

	template <typename T, typename F, size_t N, std::enable_if_t<std::is_convertible<F, T>::value>* = nullptr>
	Vector<T, N> operator/(Vector<T, N> v, F const f)
	{
		v /= static_cast<T>(f);
		return v;
	}

	using Vector2f = Vector<float, 2>;
	using Vector3f = Vector<float, 3>;
	using Vector2d = Vector<double, 2>;
	using Vector3d = Vector<double, 3>;
}

#endif // VECTOR_H
