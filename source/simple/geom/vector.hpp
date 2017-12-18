#ifndef SIMPLE_GEOM_VECTOR_HPP
#define SIMPLE_GEOM_VECTOR_HPP

#include <array>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <ostream>
#include <numeric>

#include "simple/support/logic.hpp"
#include "simple/support/array.hpp"
#include "simple/support/array_utils.hpp"

namespace simple::geom
{

	template <typename Coordinate = float, size_t Dimensions = 2>
	class vector
	{
		public:
		using array = support::array<Coordinate, Dimensions>;
		using coordinate_type = Coordinate;
		using value_type = Coordinate;
		static constexpr size_t dimensions = Dimensions;

		static constexpr vector zero() { return vector{}; }
		static constexpr vector one() { return vector{support::filled_array<Dimensions>(value_type{1})}; }

		enum : size_t
		{
			x_index,
			y_index,
			z_index,
			w_index
		};

		template <size_t index>
		static constexpr vector unit()
		{
			static_assert(index < dimensions, "");
			return vector{support::make_array<dimensions>(init_unit<index>)};
		}

		static constexpr vector unit(size_t index)
		{
			vector ret{};
			ret[index] = value_type{1};
			return ret;
		}

		static constexpr vector i() { return unit<x_index>(); }
		static constexpr vector j() { return unit<y_index>(); }
		static constexpr vector k() { return unit<z_index>(); }
		static constexpr vector l() { return unit<w_index>(); }

		private:
		// Coordinate raw[Dimensions];
		array raw;

		template <size_t dimension>
		static constexpr value_type init_unit(size_t i)
		{
			return (i == dimension) ? value_type{1} : value_type{};
		}

		public:

		vector() = default;

		template <typename... Coordinates,
				 typename support::enable_if_all<std::is_convertible<Coordinates, Coordinate>::value...> *...>
		constexpr vector(Coordinates&&... coordinates)
			: raw {std::forward<Coordinates>(coordinates)...}
		{
			static_assert(sizeof...(Coordinates) == Dimensions, " Dimension mismatch. ");
		}

		template <typename Another,
				 typename support::enable_if_all<
				 	!std::is_same<Another, vector>::value,
				 	std::is_convertible<typename Another::coordinate_type, Coordinate>::value >
					*...>
		explicit vector(const Another& another)
		{
			static_assert(Another::dimensions == Dimensions, " Dimension mismatch. ");
			std::copy(another.begin(), another.end(), begin());
		}

		explicit constexpr vector(const array & coordinates) : raw(coordinates)
		{
		}

		explicit constexpr vector(array && coordinates) : raw(std::move(coordinates))
		{
		}

		explicit operator const array & () const noexcept
		{
			return raw;
		}

		explicit operator array & () noexcept
		{
			return raw;
		}

		explicit operator array && () && noexcept
		{
			return std::move(raw);
		}

		template <typename Another>
		Another mutantClone(const std::function<typename Another::coordinate_type(vector::coordinate_type)> & method) const&
		{
			static_assert(Another::dimensions == Dimensions, " Dimension mismatch. ");
			Another another;
			std::transform(begin(), end(), another.begin(), method);
			return another;
		}

		constexpr bool operator==(const vector & another) const
		{
			return raw == another.raw;
		}

		constexpr bool operator!=(const vector & another) const
		{
			return raw != another.raw;
		}

		constexpr bool operator > (const vector & another) const
		{
			bool ret = true;
			for(size_t i = 0; i < Dimensions; ++i)
				ret = ret && (raw[i] > another.raw[i]);
			return ret;
		}

		constexpr bool operator >= (const vector & another) const
		{
			bool ret = true;
			for(size_t i = 0; i < Dimensions; ++i)
				ret = ret && (raw[i] >= another.raw[i]);
			return ret;
		}

		constexpr bool operator < (const vector & another) const
		{
			bool ret = true;
			for(size_t i = 0; i < Dimensions; ++i)
				ret = ret && (raw[i] < another.raw[i]);
			return ret;
		}

		constexpr bool operator <= (const vector & another) const
		{
			bool ret = true;
			for(size_t i = 0; i < Dimensions; ++i)
				ret = ret && (raw[i] <= another.raw[i]);
			return ret;
		}

		constexpr const coordinate_type & operator[](size_t dimension) const&
		{
			return raw[dimension];
		}

		constexpr coordinate_type & operator[](size_t dimension) &
		{
			return raw[dimension];
		}

		// NOTE: not sure about these
		typename array::const_iterator begin() const noexcept { return std::begin(raw); }
		typename array::const_iterator end() const noexcept { return std::end(raw); }
		typename array::iterator begin() noexcept { return std::begin(raw); }
		typename array::iterator end() noexcept { return std::end(raw); }

		vector& add(const Coordinate & scalar) const&
		{
		}

		vector& add(const vector & another) const&
		{
		}

		vector operator+(const Coordinate & scalar) const&
		{
			vector result;
			std::transform(begin(), end(), result.begin(),
					[&scalar](const Coordinate & coord){ return coord + scalar; } );
			return result;
		}

		vector operator-(const Coordinate & scalar) const&
		{
			vector result;
			std::transform(begin(), end(), result.begin(),
					[&scalar](const Coordinate & coord){ return coord - scalar; } );
			return result;
		}

		constexpr vector operator*(const Coordinate & scalar) const&
		{
			vector result{};
			for(size_t i = 0; i < Dimensions; ++i)
				result[i] = raw[i] * scalar;
			// std::transform(begin(), end(), result.begin(),
			// 		[&scalar](const Coordinate & coord){ return coord * scalar; } );
			return result;
		}

		vector operator/(const Coordinate & scalar) const&
		{
			vector result;
			std::transform(begin(), end(), result.begin(),
					[&scalar](const Coordinate & coord){ return coord / scalar; } );
			return result;
		}

		vector operator%(const Coordinate & scalar) const&
		{
			vector result;
			std::transform(begin(), end(), result.begin(),
					[&scalar](const Coordinate & coord){ return coord % scalar; } );
			return result;
		}

		vector operator+(const vector & another) const&
		{
			vector result;
			std::transform(begin(), end(), another.begin(), result.begin(), std::plus<>());
			return result;
		}

		vector operator-(const vector & another) const&
		{
			vector result;
			std::transform(begin(), end(), another.begin(), result.begin(), std::minus<>());
			return result;
		}

		vector operator*(const vector & another) const&
		{
			vector result;
			std::transform(begin(), end(), another.begin(), result.begin(), std::multiplies<>());
			return result;
		}

		vector operator/(const vector & another) const&
		{
			vector result;
			std::transform(begin(), end(), another.begin(), result.begin(), std::divides<>());
			return result;
		}

		vector operator%(const vector & another) const&
		{
			vector result;
			std::transform(begin(), end(), another.begin(), result.begin(), std::modulus<>());
			return result;
		}

		vector operator-() const&
		{
			// return vector::zero - *this; // i wish
			vector result;
			std::transform(begin(), end(), result.begin(), std::negate<>());
			return result;
		}

		vector & operator++()
		{
			for(auto& coord : raw) ++coord;
			return *this;
		}

		vector & operator--()
		{
			for(auto& coord : raw) --coord;
			return *this;
		}

		vector & operator+=(const Coordinate & scalar)
		{
			for(auto& coord : raw) coord += scalar;
			return *this;
		}

		vector & operator-=(const Coordinate & scalar)
		{
			for(auto& coord : raw) coord -= scalar;
			return *this;
		}

		vector & operator*=(const Coordinate & scalar)
		{
			for(auto& coord : raw) coord *= scalar;
			return *this;
		}

		vector & operator/=(const Coordinate & scalar)
		{
			for(auto& coord : raw) coord /= scalar;
			return *this;
		}

		vector & operator%=(const Coordinate & scalar)
		{
			for(auto& coord : raw) coord %= scalar;
			return *this;
		}

		vector operator++(int) &
		{
			vector temp;
			std::transform(begin(), end(), temp.begin(),
					[](Coordinate & coord) { return coord++; });
			// for(size_t i = 0; i < Dimensions; ++i) // better??
			// 	temp.raw[i] = raw[i]++;
			return temp;
		}

		vector operator--(int) &
		{
			vector temp;
			std::transform(begin(), end(), temp.begin(),
					[](Coordinate & coord) { return coord--; });
			// for(size_t i = 0; i < Dimensions; ++i) // better??
			// 	temp.raw[i] = raw[i]--;
			return temp;
		}

		vector & operator+=(const vector & another)
		{
			// return *this = *this + another // keep wishing
			for(size_t i = 0; i < Dimensions; ++i)
			{
				raw[i] += another.raw[i];
			}
			return *this;
		}

		vector & operator-=(const vector & another)
		{
			// return *this = *this - another // keep wishing
			for(size_t i = 0; i < Dimensions; ++i)
			{
				raw[i] -= another.raw[i];
			}
			return *this;
		}

		vector & operator*=(const vector & another)
		{
			// return *this = *this * another // keep wishing
			for(size_t i = 0; i < Dimensions; ++i)
			{
				raw[i] *= another.raw[i];
			}
			return *this;
		}

		vector & operator/=(const vector & another)
		{
			// return *this = *this / another // keep wishing
			for(size_t i = 0; i < Dimensions; ++i)
			{
				raw[i] /= another.raw[i];
			}
			return *this;
		}

		vector & operator%=(const vector & another)
		{
			// return *this = *this / another // keep wishing
			for(size_t i = 0; i < Dimensions; ++i)
			{
				raw[i] %= another.raw[i];
			}
			return *this;
		}

		template<typename AnotherComponent>
		Coordinate operator()(const vector<AnotherComponent, Dimensions> & another) const
		{
			return std::inner_product(begin(), end(), another.begin(), Coordinate());
		}

		// template <size_t N, size_t M>
		// vector<vectorT<N>,Dimensions> operator()(const vector<vectorT<N>,M> & another) const
		// {
		// 	vector<vectorT<N>,Dimensions> temp;
		// 	for(size_t i = 0; i < Dimensions; i++)
		// 	{
		// 		temp[i] = (*this)[i](another);
		// 	}
		// 	return temp;
		// }

		constexpr Coordinate& x()
		{
			static_assert( Dimensions > x_index );
			return raw[x_index];
		}

		constexpr const Coordinate& x() const
		{
			static_assert( Dimensions > x_index );
			return raw[x_index];
		}

		constexpr Coordinate& y()
		{
			static_assert( Dimensions > y_index );
			return raw[y_index];
		}

		constexpr const Coordinate& y() const
		{
			static_assert( Dimensions > y_index );
			return raw[y_index];
		}

		constexpr Coordinate& z()
		{
			static_assert( Dimensions > z_index );
			return raw[z_index];
		}

		constexpr const Coordinate& z() const
		{
			static_assert( Dimensions > z_index );
			return raw[z_index];
		}

		constexpr Coordinate& w()
		{
			static_assert( Dimensions > w_index );
			return raw[w_index];
		}

		constexpr const Coordinate& w() const
		{
			static_assert( Dimensions > w_index );
			return raw[w_index];
		}

	};

	template <typename Coordinate = float, size_t Dimensions = 2>
	vector<Coordinate, Dimensions> operator+(const Coordinate & scalar, const vector<Coordinate, Dimensions> & pnt)
	{
		vector<Coordinate, Dimensions> result;
		std::transform(pnt.begin(), pnt.end(), result.begin(),
				[&scalar](const Coordinate & coord){ return scalar + coord; } );
		return result;
	}

	template <typename Coordinate = float, size_t Dimensions = 2>
	vector<Coordinate, Dimensions> operator*(const Coordinate & scalar, const vector<Coordinate, Dimensions> & pnt)
	{
		vector<Coordinate, Dimensions> result;
		std::transform(pnt.begin(), pnt.end(), result.begin(),
				[&scalar](const Coordinate & coord){ return scalar * coord; } );
		return result;
	}

	template<typename Coordinate, size_t Dimensions>
	std::ostream & operator<<(std::ostream & out, const vector<Coordinate, Dimensions> & vector)
	{
		out << '(';

		constexpr size_t last = Dimensions - 1;
		for(size_t i = 0; i < last; ++i)
		{
			out << vector[i] << ", ";
		}

		out << vector[last] << ')';

		return out;
	}

	template<typename Coordinate, size_t N, size_t M>
	std::ostream & operator<<(std::ostream & out, const vector<vector<Coordinate, N>, M> & vector)
	{
		for(size_t i = 0; i < N; ++i)
			out << "---";
		out << "\n";

		for(size_t i = 0; i < M; ++i)
		{
			out << vector[i] << " \n";
		}

		for(size_t i = 0; i < N; ++i)
			out << "---";
		out << "\n";

		return out;
	}

} // namespace simple::geom

#endif /* end of include guard */
