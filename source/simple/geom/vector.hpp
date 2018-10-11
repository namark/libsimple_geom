#ifndef SIMPLE_GEOM_VECTOR_HPP
#define SIMPLE_GEOM_VECTOR_HPP

#include <algorithm>
#include <type_traits>
#include <ostream>
#include <numeric>

#include "simple/support/logic.hpp"
#include "simple/support/carcdr.hpp"
#include "simple/support/array.hpp"
#include "simple/support/array_utils.hpp"
#include "simple/support/array_operators.hpp"

namespace simple::geom
{

	template <typename Coordinate = float, size_t Dimensions = 2,
			 typename Order = std::make_index_sequence<Dimensions>,
			 std::enable_if_t<Order::size() == Dimensions>* = nullptr>
	class vector
	{
		public:
		using array = support::array<Coordinate, Dimensions>;
		using coordinate_type = Coordinate;
		using order = Order;
		using value_type = Coordinate;
		static constexpr size_t dimensions = Dimensions;

		static constexpr vector zero() { return vector{}; }
		static constexpr vector one() { return vector{support::filled_array<Dimensions>(value_type{1})}; }
		static constexpr vector one(const Coordinate& scaler) { return vector{support::filled_array<Dimensions>(scaler)}; }

		enum : size_t
		{
			non_index = std::numeric_limits<size_t>::max(),
			new_index = non_index,
			x_index = support::car<Order, 0>(non_index),
			y_index = support::car<Order, 1>(non_index),
			z_index = support::car<Order, 2>(non_index),
			w_index = support::car<Order, 3>(non_index)
		};

		template <size_t index>
		static constexpr vector unit()
		{
			return unit<index>(Coordinate{1});
		}

		template <size_t index>
		static constexpr vector unit(const Coordinate& one)
		{
			return unit<index>(one, Coordinate{});
		}

		template <size_t index>
		static constexpr vector unit(const Coordinate& one, const Coordinate& zero)
		{
			static_assert(index < dimensions, "");
			return vector{support::make_array<dimensions>([one, zero](size_t i)
				{ return index == i ? one : zero; } )};
		}

		static constexpr vector unit(size_t index)
		{
			return unit(index, Coordinate{1});
		}

		static constexpr vector unit(size_t index, const Coordinate& one)
		{
			vector ret{};
			ret[index] = one;
			return ret;
		}

		static constexpr vector unit(size_t index, const Coordinate& one, const Coordinate& zero)
		{
			vector ret = vector::one(zero);
			ret[index] = one;
			return ret;
		}

		static constexpr vector i() { return unit<x_index>(); }
		static constexpr vector j() { return unit<y_index>(); }
		static constexpr vector k() { return unit<z_index>(); }
		static constexpr vector l() { return unit<w_index>(); }
		static constexpr vector i(const Coordinate& scaler) { return unit<x_index>(scaler); }
		static constexpr vector j(const Coordinate& scaler) { return unit<y_index>(scaler); }
		static constexpr vector k(const Coordinate& scaler) { return unit<z_index>(scaler); }
		static constexpr vector l(const Coordinate& scaler) { return unit<w_index>(scaler); }

		private:
		// Coordinate raw[Dimensions];
		array raw;

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

		template <typename Another, std::enable_if_t<
			Another::dimensions == Dimensions
			&& !std::is_same<Another, vector>::value
			&& !std::is_base_of<vector, Another>::value
			&& std::is_convertible<typename Another::coordinate_type, Coordinate>::value
		>* = nullptr>
		struct is_convertible_to_me {};

		template <typename Another, size_t dimension = Dimensions - 1>
		constexpr void set_in_order(const Another& another)
		{
			if constexpr (dimension > 0)
				set_in_order<Another, dimension - 1>(another);
			get<dimension>() = another.template get<dimension>();
		}

		public:

		vector() = default;

		template <typename... Coordinates,
				 typename std::enable_if_t<sizeof...(Coordinates) == Dimensions> * = nullptr,
				 typename support::enable_if_all<std::is_convertible<Coordinates, Coordinate>::value...> * = nullptr>
		constexpr vector(Coordinates&&... coordinates)
			: raw {std::forward<Coordinates>(coordinates)...}
		{}

		template <typename Another, is_convertible_to_me<Another>* = nullptr,
		std::enable_if_t<std::is_same_v<typename Another::order, Order>> *...>
		explicit vector(const Another& another)
		{
			std::copy(another.begin(), another.end(), begin());
		}

		template <typename Another, is_convertible_to_me<Another>* = nullptr,
	 	std::enable_if_t<!std::is_same_v<typename Another::order, Order>> *...>
		constexpr explicit vector(const Another& another) : raw{}
		{
			set_in_order<Another>(another);
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

		template <size_t dimension>
		constexpr const coordinate_type & get() const&
		{
			static_assert(dimension < Dimensions);
			constexpr size_t index = support::car<Order, dimension>();
			return raw[index];
		}

		template <size_t dimension>
		constexpr coordinate_type & get() &
		{
			static_assert(dimension < Dimensions);
			constexpr size_t index = support::car<Order, dimension>();
			return raw[index];
		}

		constexpr const coordinate_type & operator[](size_t dimension) const&
		{
			return raw[dimension];
		}

		constexpr coordinate_type & operator[](size_t dimension) &
		{
			return raw[dimension];
		}

		constexpr auto begin() noexcept { return std::begin(raw); }
		constexpr auto end() noexcept { return std::end(raw); }
		constexpr auto begin() const noexcept { return std::cbegin(raw); }
		constexpr auto end() const noexcept { return std::cend(raw); }
		constexpr auto cbegin() const noexcept { return std::cbegin(raw); }
		constexpr auto cend() const noexcept { return std::cend(raw); }

		constexpr auto rbegin() noexcept { return std::rbegin(raw); }
		constexpr auto rend() noexcept { return std::rend(raw); }
		constexpr auto rbegin() const noexcept { return std::crbegin(raw); }
		constexpr auto rend() const noexcept { return std::crend(raw); }
		constexpr auto crbegin() const noexcept { return std::crbegin(raw); }
		constexpr auto crend() const noexcept { return std::crend(raw); }

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

		constexpr vector& floor()
		{
			using std::floor;
			for(auto&& coord : raw)
				coord = floor(coord);
			return *this;
		}

		constexpr vector& ceil()
		{
			using std::ceil;
			for(auto&& coord : raw)
				coord = ceil(coord);
			return *this;
		}

		constexpr vector& round()
		{
			using std::round;
			for(auto&& coord : raw)
				coord = round(coord);
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

		template <typename T, typename = std::nullptr_t>
		struct can_apply_s { constexpr static bool value = false; };
		template <typename T>
		struct can_apply_s<T, decltype(std::declval<vector>()(std::declval<T>()), nullptr)> { constexpr static bool value = true; };
		template <typename T>
		constexpr static bool can_apply = can_apply_s<T>::value;

		// square matrix multiplication and matrix-vector multiplication/dot product fusion mutant operator
		template<typename AnotherComponent, typename Return = std::conditional_t<can_apply<AnotherComponent>, vector, Coordinate>>
		constexpr Return operator()(const vector<AnotherComponent, Dimensions> & another) const
		{
			Return ret{};
			for(size_t i = 0; i < Dimensions; ++i)
				if constexpr (can_apply<AnotherComponent>)
					ret[i] = (*this)(another[i]);
				else
					ret += (*this)[i] * another[i];
			return ret;
		}

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

	template <typename C, size_t D, typename O>
	constexpr
	vector<C,D,O> min(const vector<C,D,O> & one, const vector<C,D,O> & other)
	{
		auto result = one;
		return result.min(other);
	}

	template <typename C, size_t D, typename O>
	constexpr
	vector<C,D,O> max(const vector<C,D,O> & one, const vector<C,D,O> & other)
	{
		auto result = one;
		return result.max(other);
	}

	template <typename C, size_t D, typename O>
	constexpr
	vector<C,D,O> clamp(vector<C,D,O> v, const vector<C,D,O> & lo, const vector<C,D,O> & hi)
	{
		v.clamp(lo, hi);
		return v;
	}

	template <typename C, size_t D, typename O>
	constexpr
	vector<C,D,O> floor(vector<C,D,O> v)
	{
		v.floor();
		return v;
	}

	template <typename C, size_t D, typename O>
	constexpr
	vector<C,D,O> ceil(vector<C,D,O> v)
	{
		v.ceil();
		return v;
	}

	template <typename C, size_t D, typename O>
	constexpr
	vector<C,D,O> round(vector<C,D,O> v)
	{
		v.round();
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

	template<typename Coordinate, size_t Dimensions, typename Order>
	std::ostream & operator<<(std::ostream & out, const vector<Coordinate, Dimensions, Order> & vector)
	{
		out << '(';

		constexpr size_t last = Dimensions - 1;
		for(size_t i = 0; i < last; ++i)
			out << vector[i] << ", ";

		out << vector[last] << ')';

		return out;
	}

	template<typename Coordinate, size_t N, size_t M, typename O1, typename O2>
	std::ostream & operator<<(std::ostream & out, const vector<vector<Coordinate, N, O1>, M, O2> & vector)
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
	template<typename C, size_t D, typename O>
	struct support::define_array_operators<geom::vector<C,D,O>> :
	public support::trivial_array_accessor<geom::vector<C,D,O>, geom::vector<C,D,O>::dimensions>
	{
		constexpr static auto enabled_operators = array_operator::all;
		constexpr static auto enabled_right_element_operators = array_operator::binary | array_operator::in_place;
		constexpr static auto enabled_left_element_operators = array_operator::binary
			^ array_operator::lshift
			^ array_operator::rshift;
	};
} // namespace simple

template<typename T, size_t C, typename O>
class std::numeric_limits<simple::geom::vector<T,C,O>>
{
	using vec = simple::geom::vector<T,C,O>;
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
