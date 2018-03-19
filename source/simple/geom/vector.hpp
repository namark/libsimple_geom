#ifndef SIMPLE_GEOM_VECTOR_HPP
#define SIMPLE_GEOM_VECTOR_HPP

#include <algorithm>
#include <type_traits>
#include <ostream>
#include <numeric>

#include "simple/support/logic.hpp"
#include "simple/support/array.hpp"
#include "simple/support/array_utils.hpp"
#include "simple/support/array_operators.hpp"

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

		template<typename Vector, size_t n>
		constexpr void set_mixed_index(Vector&) const
		{}

		template<typename Vector, size_t n>
		constexpr void set_mixed_index(Vector&, const Coordinate&) const
		{}

		template<typename Vector, size_t n, size_t index, size_t... Rest>
		constexpr void set_mixed_index(Vector& vec) const
		{
			static_assert(index < dimensions, " Invalid mix index. ");
			vec[n] = raw[index];
			set_mixed_index<Vector, n+1, Rest...>(vec);
		}

		template<typename Vector, size_t n, size_t index, size_t... Rest,
		std::enable_if_t<index < Dimensions>* = nullptr>
		constexpr void set_mixed_index(Vector& vec, const Coordinate& default_value) const
		{
			vec[n] = raw[index];
			set_mixed_index<Vector, n+1, Rest...>(vec, default_value);
		}

		template<typename Vector, size_t n, size_t index, size_t... Rest,
		std::enable_if_t<index >= Dimensions>* = nullptr>
		constexpr void set_mixed_index(Vector& vec, const Coordinate& default_value) const
		{
			vec[n] = default_value;
			set_mixed_index<Vector, n+1, Rest...>(vec, default_value);
		}

		public:

		vector() = default;

		template <typename... Coordinates,
				 typename std::enable_if_t<sizeof...(Coordinates) == Dimensions> * = nullptr,
				 typename support::enable_if_all<std::is_convertible<Coordinates, Coordinate>::value...> * = nullptr>
		constexpr vector(Coordinates&&... coordinates)
			: raw {std::forward<Coordinates>(coordinates)...}
		{}

		template <typename Another,
				 typename support::enable_if_all<
					Another::dimensions == Dimensions,
				 	!std::is_same<Another, vector>::value && !std::is_base_of<vector, Another>::value,
				 	std::is_convertible<typename Another::coordinate_type, Coordinate>::value >
					*...>
		explicit vector(const Another& another)
		{
			std::copy(another.begin(), another.end(), begin());
		}

		explicit constexpr vector(const array & coordinates) : raw(coordinates)
		{
		}

		explicit constexpr vector(array && coordinates) : raw(std::move(coordinates))
		{
		}

		explicit operator const array () const noexcept
		{
			return raw;
		}

		template <typename Another>
		Another mutantClone(typename Another::coordinate_type(*method)(const vector::coordinate_type&)) const&
		{
			static_assert(Another::dimensions == Dimensions, " Dimension mismatch. ");
			Another another;
			std::transform(begin(), end(), another.begin(), method);
			return another;
		}

		template<size_t... CoordinateIndices, typename Mixed = vector<Coordinate, sizeof...(CoordinateIndices)> >
		constexpr Mixed mix() const
		{
			Mixed result{};
			set_mixed_index<Mixed, 0, CoordinateIndices...>(result);
			return result;
		}

		template<size_t... CoordinateIndices, typename Mixed = vector<Coordinate, sizeof...(CoordinateIndices)> >
		constexpr Mixed mix(const Coordinate& default_value) const
		{
			Mixed result{};
			set_mixed_index<Mixed, 0, CoordinateIndices...>(result, default_value);
			return result;
		}

		template <size_t N, typename Mixed = vector<Coordinate, N>>
		constexpr Mixed mix(const support::array<Coordinate,N>& indices) const
		{
			Mixed result{};
			size_t index{};
			for(size_t i = 0; i < N; ++i)
			{
				index = indices[i];
				if(index >= Dimensions)
					throw std::logic_error("simple::geom::vector invalid mix index");
				result[i] = raw[index];
			}
			return result;
		}

		template <size_t N, typename Mixed = vector<Coordinate, N>>
		constexpr Mixed mix(const support::array<Coordinate,N>& indices, const Coordinate& default_value) const
		{
			Mixed result{};
			size_t index{};
			for(size_t i = 0; i < N; ++i)
			{
				index = indices[i];
				result[i] = index < Dimensions ? raw[index] : default_value;
			}
			return result;
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
		constexpr typename array::const_iterator begin() const noexcept { return std::begin(raw); }
		constexpr typename array::const_iterator end() const noexcept { return std::end(raw); }
		constexpr typename array::iterator begin() noexcept { return std::begin(raw); }
		constexpr typename array::iterator end() noexcept { return std::end(raw); }

		constexpr vector& min(const vector& other)
		{
			using std::min;
			for(size_t i = 0; i < dimensions; ++i)
				raw[i] = min(raw[i], other[i]);
			return *this;
		}

		constexpr vector& max(const vector& other)
		{
			using std::max;
			for(size_t i = 0; i < dimensions; ++i)
				raw[i] = max(raw[i], other[i]);
			return *this;
		}

		constexpr vector& clamp(const vector& lo, const vector& hi)
		{
			using std::clamp;
			for(size_t i = 0; i < dimensions; ++i)
				raw[i] = clamp(raw[i], lo[i], hi[i]);
			return *this;
		}

		constexpr vector & operator++()
		{
			for(auto& coord : raw) ++coord;
			return *this;
		}

		constexpr vector & operator--()
		{
			for(auto& coord : raw) --coord;
			return *this;
		}

		constexpr vector operator++(int) &
		{
			vector temp{};
			for(size_t i = 0; i < Dimensions; ++i)
				temp.raw[i] = raw[i]++;
			return temp;
		}

		constexpr vector operator--(int) &
		{
			vector temp{};
			for(size_t i = 0; i < Dimensions; ++i)
				temp.raw[i] = raw[i]--;
			return temp;
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

		constexpr vector<Coordinate,2> xy() const
		{
			return mix<x_index, y_index>();
		}

		constexpr vector<Coordinate,3> xyz() const
		{
			return mix<x_index, y_index, z_index>();
		}

		constexpr vector<Coordinate,3> xyz(const Coordinate& default_value) const
		{
			return mix<x_index, y_index, z_index>(default_value);
		}

	};

	template <typename C, size_t D>
	constexpr
	vector<C,D> min(const vector<C,D> & one, const vector<C,D> & other)
	{
		vector<C,D> result = one;
		return result.min(other);
	}

	template <typename C, size_t D>
	constexpr
	vector<C,D> max(const vector<C,D> & one, const vector<C,D> & other)
	{
		vector<C,D> result = one;
		return result.max(other);
	}

	template <typename C, size_t D>
	constexpr
	vector<C,D> clamp(vector<C,D> v, const vector<C,D> & lo, const vector<C,D> & hi)
	{
		v.clamp(lo, hi);
		return v;
	}

	// for ADL to find these
	using ::operator+;
	using ::operator-;
	using ::operator*;
	using ::operator/;
	using ::operator%;
	using ::operator&;
	using ::operator|;
	using ::operator^;
	using ::operator+=;
	using ::operator-=;
	using ::operator*=;
	using ::operator/=;
	using ::operator%=;
	using ::operator&=;
	using ::operator|=;
	using ::operator^=;

	template<typename Coordinate, size_t Dimensions>
	std::ostream & operator<<(std::ostream & out, const vector<Coordinate, Dimensions> & vector)
	{
		out << '(';

		constexpr size_t last = Dimensions - 1;
		for(size_t i = 0; i < last; ++i)
			out << vector[i] << ", ";

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
			out << vector[i] << " \n";

		for(size_t i = 0; i < N; ++i)
			out << "---";
		out << "\n";

		return out;
	}

} // namespace simple::geom

namespace simple
{
	template<typename C, size_t D>
	struct support::define_array_operators<geom::vector<C,D>> :
	public support::trivial_array_accessor<geom::vector<C,D>, geom::vector<C,D>::dimensions>
	{
		constexpr static auto enabled_operators = array_operator::all;
		constexpr static auto enabled_right_element_operators = array_operator::binary | array_operator::in_place;
		constexpr static auto enabled_left_element_operators = array_operator::binary
			^ array_operator::lshift
			^ array_operator::rshift;
	};
} // namespace simple

template<typename T, size_t C>
class std::numeric_limits<simple::geom::vector<T,C>>
{
	using vec = simple::geom::vector<T,C>;
	using limits = std::numeric_limits<T>;
	public:
	constexpr static bool is_specialized = limits::is_specialized;

	constexpr static vec min()
	{
		static_assert(limits::is_specialized);
		vec m{};
		for(auto&& c : m)
			c = limits::min();
		return m;
	}

	constexpr static vec max()
	{
		static_assert(limits::is_specialized);
		vec m{};
		for(auto&& c : m)
			c = limits::max();
		return m;
	}
};


#endif /* end of include guard */
