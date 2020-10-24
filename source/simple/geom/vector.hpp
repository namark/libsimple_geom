// TODO: this file is getting huge, need to break it up somehow
#ifndef SIMPLE_GEOM_VECTOR_HPP
#define SIMPLE_GEOM_VECTOR_HPP

#include <algorithm>
#include <type_traits>
#include <ostream>
#include <numeric>
#include <cassert>

#include "simple/support/logic.hpp"
#include "simple/support/carcdr.hpp"
#include "simple/support/array.hpp"
#include "simple/support/array_utils.hpp"
#include "simple/support/array_operators.hpp"

namespace simple::geom
{

	template <typename Type, size_t Size>
	struct vector_array
	{
		using type = support::array<Type, Size>;
	};

	template <typename Type, size_t Size>
	using vector_array_t = typename vector_array<Type, Size>::type;

	// TODO; in an optional header
	// template <size_t Size>
	// struct vector_array<unsigned int, Size>
	// {
	// 	using value_type = unsigned int;
	// 	typedef value_type type __attribute__ ((vector_size (Size * sizeof(value_type))));
	// };
    //
	// template <size_t Size>
	// struct vector_array<int, Size>
	// {
	// 	using value_type = int;
	// 	typedef value_type type __attribute__ ((vector_size (Size * sizeof(value_type))));
	// };

	template <typename Coordinate, size_t Dimensions,
			typename Order = std::make_index_sequence<Dimensions>,
			std::enable_if_t<Order::size() == Dimensions>* = nullptr>
	class vector
	{
		public:
		using array = vector_array_t<Coordinate, Dimensions>;
		using coordinate_type = Coordinate;
		using order = Order;
		using value_type = Coordinate;
		static constexpr size_t dimensions = Dimensions;

		class meta
		{
			template <typename T, typename = std::nullptr_t>
			struct has_dimesntions_s { constexpr static bool value = false; };
			template <typename T>
			struct has_dimesntions_s<T, decltype(void(T::dimensions), nullptr)> { constexpr static bool value = true; };
			template <typename T>
			constexpr static bool has_dimesntions = has_dimesntions_s<T>::value;

			public:
			template <typename C = coordinate_type,
				std::enable_if_t<has_dimesntions<C>>* = nullptr>
			constexpr static size_t depth()
			{
				return depth<typename C::coordinate_type>() + 1;
			}
			template <typename C = coordinate_type,
				std::enable_if_t<not has_dimesntions<C>>* = nullptr>
			constexpr static size_t depth()
			{
				return 1;
			}

			private:
			template <size_t DepthIndex = depth(), typename Vector = vector>
			static constexpr size_t get_sub_dimentions()
			{
				static_assert( DepthIndex < depth(), "Invalid depth" );
				if constexpr (DepthIndex == 0)
					return Vector::dimensions;
				else return get_sub_dimentions<DepthIndex-1, typename Vector::coordinate_type>();
			}

			template <typename O, typename SizeType = size_t, size_t DepthIndex = 0, size_t Depth = depth()>
			static constexpr void set_sub_dimentions(vector<SizeType,Depth,O>& out)
			{
				out[Depth - 1 - DepthIndex] = get_sub_dimentions<DepthIndex>();
				if constexpr (DepthIndex+1 < Depth)
					set_sub_dimentions<O,SizeType,DepthIndex+1>(out);
			}

			template <typename SizeType = size_t, size_t Depth = depth()>
			static constexpr auto get_dimensions()
			{
				vector<SizeType,Depth> ret{};
				set_sub_dimentions(ret);
				return ret;
			}

			public:
			template <typename SizeType = size_t, size_t Depth = meta::depth()> // have to explicitly qualify for depth gcc-7
			static constexpr vector<SizeType,Depth> dimensions = get_dimensions<SizeType,Depth>();

			// this little convenience crashes all clangs currently supporting c++17 (5,6,7,8,9,10)
			// static constexpr auto size = dimensions<>;

			// unused typename to work around gcc bug
			// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
			template <typename, size_t Depth = depth(), typename Enabled = void>
			struct get_coordinate;
			template <typename U>
			struct get_coordinate<U, 1, void>
			{ using type = vector::coordinate_type; };
			template <typename U, size_t Depth>
			struct get_coordinate<U, Depth, std::enable_if_t<(Depth > 1)>>
			{ using type = typename vector::coordinate_type::meta:: template get_coordinate<U, Depth -1>::type; };

			using coordinate_type = typename get_coordinate<void, depth()>::type;

		};

		template <typename NewCoord, typename Meta = meta, typename Enabled = void>
		struct map_coordinate;
		template <typename NewCoord, typename Meta>
		struct map_coordinate<NewCoord, Meta, std::enable_if_t<Meta::depth() <= 1>>
		{ using type = vector<NewCoord, dimensions, order>; };
		template <typename NewCoord, typename Meta>
		struct map_coordinate<NewCoord, Meta, std::enable_if_t<(Meta::depth() > 1)>>
		{ using type =
			vector<typename Coordinate::template map_coordinate<NewCoord>::type, dimensions, order>; };

		template <typename NewCoord>
		using map_coordinate_t = typename map_coordinate<NewCoord>::type;


		[[nodiscard]]
		static constexpr vector zero() { return vector{}; }
		[[nodiscard]]
		static constexpr vector one() { return vector{support::filled_array<Dimensions>(value_type{1})}; }
		[[nodiscard]]
		static constexpr vector one(const Coordinate& scaler) { return vector{support::filled_array<Dimensions>(scaler)}; }

		enum : size_t
		{
			non_index = std::numeric_limits<size_t>::max(),
			new_index = non_index,
			x_index = support::car<Order, 0, non_index>,
			y_index = support::car<Order, 1, non_index>,
			z_index = support::car<Order, 2, non_index>,
			w_index = support::car<Order, 3, non_index>
		};

		template <size_t index>
		[[nodiscard]]
		static constexpr vector unit()
		{
			return unit<index>(Coordinate{1});
		}

		template <size_t index>
		[[nodiscard]]
		static constexpr vector unit(const Coordinate& one)
		{
			return unit<index>(one, Coordinate{});
		}

		template <size_t index>
		[[nodiscard]]
		static constexpr vector unit(const Coordinate& one, const Coordinate& zero)
		{
			static_assert(index < dimensions, "");
			return vector{support::make_array<dimensions>([one, zero](size_t i)
				{ return index == i ? one : zero; } )};
		}

		[[nodiscard]]
		static constexpr vector unit(size_t index)
		{
			return unit(index, Coordinate{1});
		}

		[[nodiscard]]
		static constexpr vector unit(size_t index, const Coordinate& one)
		{
			vector ret{};
			ret[index] = one;
			return ret;
		}

		[[nodiscard]]
		static constexpr vector unit(size_t index, const Coordinate& one, const Coordinate& zero)
		{
			vector ret = vector::one(zero);
			ret[index] = one;
			return ret;
		}

		[[nodiscard]] static constexpr vector i() { return unit<x_index>(); }
		[[nodiscard]] static constexpr vector j() { return unit<y_index>(); }
		[[nodiscard]] static constexpr vector k() { return unit<z_index>(); }
		[[nodiscard]] static constexpr vector l() { return unit<w_index>(); }
		[[nodiscard]] static constexpr vector i(const Coordinate& scaler) { return unit<x_index>(scaler); }
		[[nodiscard]] static constexpr vector j(const Coordinate& scaler) { return unit<y_index>(scaler); }
		[[nodiscard]] static constexpr vector k(const Coordinate& scaler) { return unit<z_index>(scaler); }
		[[nodiscard]] static constexpr vector l(const Coordinate& scaler) { return unit<w_index>(scaler); }

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

		constexpr vector() = default;

		template <typename... Coordinates,
				typename std::enable_if_t<sizeof...(Coordinates) == Dimensions> * = nullptr,
				typename support::enable_if_all<std::is_convertible<Coordinates, Coordinate>::value...> * = nullptr>
		constexpr vector(Coordinates&&... coordinates)
			: raw {std::forward<Coordinates>(coordinates)...}
		{}

		// TODO: constexpr and forwarding
		template <typename Another, is_convertible_to_me<Another>* = nullptr,
		std::enable_if_t<std::is_same_v<typename Another::order, Order>> *...>
		constexpr explicit vector(const Another& another) : raw{}
		{
			for(size_t i = 0; i < Dimensions; ++i)
				raw[i] = another[i];
		}

		// TODO: forwarding
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

		[[nodiscard]]
		explicit constexpr operator const array () const noexcept
		{
			return raw;
		}

		template<size_t D = Dimensions, std::enable_if_t<D == 1>* = nullptr>
		[[nodiscard]]
		constexpr operator const Coordinate & () const noexcept
		{
			return raw[0];
		}

		template <typename Another>
		[[nodiscard]] [[deprecated("use mutant_clone instead")]]
		constexpr Another mutantClone(typename Another::coordinate_type(*method)(const vector::coordinate_type&)) const&
		{
			static_assert(Another::dimensions == Dimensions, " Dimension mismatch. ");
			Another another{};
			std::transform(begin(), end(), another.begin(), method);
			return another;
		}

		template <typename Function, typename AnotherCoord = std::invoke_result_t<Function, Coordinate>>
		[[nodiscard]]
		constexpr vector<AnotherCoord, Dimensions, Order> mutant_clone(Function&& transform) const&
		{
			vector<AnotherCoord, Dimensions> another{};
			for(size_t i = 0; i < Dimensions; ++i)
			{
				// std::invoke is not constexpr -_-
				another[i] = std::apply(
					std::forward<Function>(transform),
					std::forward_as_tuple((*this)[i])
				);
			}
			return another;
		}

		// TODO: mix should preserve order if mixed size >= original size
		template<size_t... CoordinateIndices, typename Mixed = vector<Coordinate, sizeof...(CoordinateIndices)> >
		[[nodiscard]]
		constexpr Mixed mix() const
		{
			Mixed result{};
			set_mixed_index<Mixed, 0, CoordinateIndices...>(result);
			return result;
		}

		template<size_t... CoordinateIndices, typename Mixed = vector<Coordinate, sizeof...(CoordinateIndices)> >
		[[nodiscard]]
		constexpr Mixed mix(const Coordinate& default_value) const
		{
			Mixed result{};
			set_mixed_index<Mixed, 0, CoordinateIndices...>(result, default_value);
			return result;
		}

		template <size_t N, typename Mixed = vector<Coordinate, N>>
		[[nodiscard]]
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
		[[nodiscard]]
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

		template <size_t N,
			std::enable_if_t<N <= Dimensions>* = nullptr>
		[[nodiscard]]
		constexpr vector<Coordinate,N> last() const
		{
			vector<Coordinate,N> result{};
			for(size_t i = 0; i < N; ++i)
				result[i] = (*this)[Dimensions-N+i];
			return result;
		}

		template <size_t N,
			std::enable_if_t<N <= Dimensions>* = nullptr>
		[[nodiscard]]
		constexpr vector<Coordinate,N> first() const
		{
			vector<Coordinate,N> result{};
			for(size_t i = 0; i < N; ++i)
				result[i] = (*this)[i];
			return result;
		}

		// NOTE: emmm... ok??
		template <typename C = Coordinate, std::enable_if_t<std::is_same_v<C,bool>>* = nullptr>
		[[nodiscard]]
		friend
		constexpr vector operator ~(vector one) noexcept
		{
			for(size_t i = 0; i < Dimensions; ++i)
				one[i] = !one[i];
			return one;
		}
		template <typename C = typename meta::coordinate_type,
			std::enable_if_t<std::is_same_v<C,bool>>* = nullptr>
		[[nodiscard]]
		friend
		constexpr vector operator !(vector one) noexcept
		{
			for(size_t i = 0; i < Dimensions; ++i)
				one[i] = !one[i];
			return one;
		}

		template <size_t dimension>
		[[nodiscard]]
		constexpr const coordinate_type & get() const&
		{
			static_assert(dimension < Dimensions);
			constexpr size_t index = support::car<Order, dimension>;
			return (*this)[index];
		}

		template <size_t dimension>
		[[nodiscard]]
		constexpr coordinate_type & get() &
		{
			static_assert(dimension < Dimensions);
			constexpr size_t index = support::car<Order, dimension>;
			return (*this)[index];
		}

		template <size_t dimension>
		[[nodiscard]]
		constexpr coordinate_type && get() &&
		{
			static_assert(dimension < Dimensions);
			constexpr size_t index = support::car<Order, dimension>;
			return std::move(*this)[index];
		}

		template <size_t dimension>
		[[nodiscard]]
		constexpr const coordinate_type && get() const&&
		{
			static_assert(dimension < Dimensions);
			constexpr size_t index = support::car<Order, dimension>;
			return (*this)[index];
		}

		[[nodiscard]] constexpr const coordinate_type &
		operator[](size_t dimension) const&
		{
			assert(dimension < Dimensions);
			return raw[dimension];
		}

		[[nodiscard]] constexpr coordinate_type &
		operator[](size_t dimension) &
		{
			assert(dimension < Dimensions);
			return raw[dimension];
		}

		[[nodiscard]] constexpr coordinate_type &&
		operator[](size_t dimension) &&
		{
			assert(dimension < Dimensions);
			return std::move(raw[dimension]);
		}

		[[nodiscard]] constexpr const coordinate_type &&
		operator[](size_t dimension) const&&
		{
			assert(dimension < Dimensions);
			return raw[dimension];
		}

		// TODO: not sure if it's ok that this is backwards
		// make sure to change meta::dimensions as well if change this
		// maybe make use of order parameter to decide
		// also definitely not ok that const and non const are so duplicated
		template <typename IndexType, size_t Size, typename O, size_t Depth = Size,
			std::enable_if_t<(Depth > 1)>* = nullptr>
		[[nodiscard]]
		constexpr auto & operator[](vector<IndexType,Size,O> index) &
		{
			return operator[](index[Depth - 1])
				.template operator[]<IndexType, Size, O, Depth-1>(index);
		}
		template <typename IndexType, size_t Size, typename O, size_t Depth,
			std::enable_if_t<Depth == 1>* = nullptr>
		[[nodiscard]]
		constexpr coordinate_type & operator[](vector<IndexType,Size,O> index) &
		{
			return operator[](index[0]);
		}

		template <typename IndexType, size_t Size, typename O, size_t Depth = Size,
			std::enable_if_t<(Depth > 1)>* = nullptr>
		[[nodiscard]]
		constexpr const auto & operator[](vector<IndexType,Size,O> index) const&
		{
			return raw[index[Depth - 1]]
				.template operator[]<IndexType, Size, O, Depth-1>(index);
		}
		template <typename IndexType, size_t Size, typename O, size_t Depth,
			std::enable_if_t<Depth == 1>* = nullptr>
		[[nodiscard]]
		constexpr const coordinate_type & operator[](vector<IndexType,Size,O> index) const&
		{
			return raw[index[0]];
		}

		[[nodiscard]] constexpr auto begin()  noexcept       { using std::begin;  return begin(raw);  }
		[[nodiscard]] constexpr auto end()    noexcept       { using std::end;    return end(raw);    }
		[[nodiscard]] constexpr auto begin()  const noexcept { using std::cbegin; return cbegin(raw); }
		[[nodiscard]] constexpr auto end()    const noexcept { using std::cend;   return cend(raw);   }
		[[nodiscard]] constexpr auto cbegin() const noexcept { using std::cbegin; return cbegin(raw); }
		[[nodiscard]] constexpr auto cend()   const noexcept { using std::cend;   return cend(raw);   }

		[[nodiscard]] constexpr auto rbegin()  noexcept       { using std::rbegin;  return rbegin(raw);  }
		[[nodiscard]] constexpr auto rend()    noexcept       { using std::rend;    return rend(raw);    }
		[[nodiscard]] constexpr auto rbegin()  const noexcept { using std::crbegin; return crbegin(raw); }
		[[nodiscard]] constexpr auto rend()    const noexcept { using std::crend;   return crend(raw);   }
		[[nodiscard]] constexpr auto crbegin() const noexcept { using std::crbegin; return crbegin(raw); }
		[[nodiscard]] constexpr auto crend()   const noexcept { using std::crend;   return crend(raw);   }

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

		constexpr vector& trunc()
		{
			using std::trunc;
			for(auto&& coord : raw)
				coord = trunc(coord);
			return *this;
		}

		constexpr vector& abs()
		{
			using std::abs;
			for(auto&& coord : raw)
				coord = abs(coord);
			return *this;
		}

		constexpr vector& signum()
		{
			using std::abs;
			for(auto&& coord : raw)
				coord = !(coord != Coordinate{}) ? Coordinate{} : coord/abs(coord);
			return *this;
		};

		[[nodiscard]]
		constexpr Coordinate magnitude() const
		{
			Coordinate result = Coordinate{};
			for(auto&& coord : raw)
				result += coord * coord;
			return result;
		}

		[[nodiscard]]
		constexpr Coordinate quadrance() const
		{
			return magnitude();
		}

		[[nodiscard]]
		constexpr Coordinate length() const
		{
			using std::sqrt;
			return sqrt(magnitude());
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

		[[nodiscard]]
		constexpr vector operator++(int) &
		{
			vector temp{};
			for(size_t i = 0; i < Dimensions; ++i)
				temp.raw[i] = raw[i]++;
			return temp;
		}

		[[nodiscard]]
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
		struct can_apply_s<T, decltype(void(std::declval<vector>()(std::declval<T>())), nullptr)> { constexpr static bool value = true; };
		template <typename T>
		constexpr static bool can_apply = can_apply_s<T>::value;

		// TODO: common declval code between this and can_apply
		template <typename T, typename = std::nullptr_t>
		struct product_result_s { using type = Coordinate; };
		template <typename T>
		struct product_result_s<T, decltype(void(std::declval<vector>()(std::declval<T>())), nullptr)> { using type = decltype(std::declval<vector>()(std::declval<T>())); };
		template <typename T>
		using product_result = typename product_result_s<T>::type;

		// matrix multiplication and matrix-vector multiplication/dot product fusion mutant operator
		template<typename AnotherComponent, size_t AnotherDimesnions, typename AnotherOrder,
			std::enable_if_t<std::is_same_v<Order,AnotherOrder> || can_apply<AnotherComponent>>* = nullptr,
			typename Return = std::conditional_t<can_apply<AnotherComponent>,
				vector<product_result<AnotherComponent>, AnotherDimesnions, AnotherOrder>,
				Coordinate
			>
		>
		[[nodiscard]]
		constexpr Return operator()(const vector<AnotherComponent, AnotherDimesnions, AnotherOrder> & another) const
		{
			Return ret{};
			for(size_t i = 0; i < AnotherDimesnions; ++i)
				if constexpr (can_apply<AnotherComponent>)
					ret[i] = (*this)(another[i]);
				else
					ret += (*this)[i] * another[i];
			return ret;
		}

		[[nodiscard]]
		constexpr Coordinate& x()
		{
			static_assert( Dimensions > x_index );
			return raw[x_index];
		}

		[[nodiscard]]
		constexpr const Coordinate& x() const
		{
			static_assert( Dimensions > x_index );
			return raw[x_index];
		}

		[[nodiscard]]
		constexpr Coordinate& y()
		{
			static_assert( Dimensions > y_index );
			return raw[y_index];
		}

		[[nodiscard]]
		constexpr const Coordinate& y() const
		{
			static_assert( Dimensions > y_index );
			return raw[y_index];
		}

		[[nodiscard]]
		constexpr Coordinate& z()
		{
			static_assert( Dimensions > z_index );
			return raw[z_index];
		}

		[[nodiscard]]
		constexpr const Coordinate& z() const
		{
			static_assert( Dimensions > z_index );
			return raw[z_index];
		}

		[[nodiscard]]
		constexpr Coordinate& w()
		{
			static_assert( Dimensions > w_index );
			return raw[w_index];
		}

		[[nodiscard]]
		constexpr const Coordinate& w() const
		{
			static_assert( Dimensions > w_index );
			return raw[w_index];
		}

		[[nodiscard]]
		constexpr vector<Coordinate,2> xy() const
		{
			return mix<x_index, y_index>();
		}

		[[nodiscard]]
		constexpr vector<Coordinate,3> xyz() const
		{
			return mix<x_index, y_index, z_index>();
		}

		[[nodiscard]]
		constexpr vector<Coordinate,3> xyz(const Coordinate& default_value) const
		{
			return mix<x_index, y_index, z_index>(default_value);
		}

	};

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> min(const vector<C,D,O> & one, const vector<C,D,O> & other)
	{
		auto result = one;
		return result.min(other);
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> max(const vector<C,D,O> & one, const vector<C,D,O> & other)
	{
		auto result = one;
		return result.max(other);
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> clamp(vector<C,D,O> v, const vector<C,D,O> & lo, const vector<C,D,O> & hi)
	{
		v.clamp(lo, hi);
		return v;
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> floor(vector<C,D,O> v)
	{
		v.floor();
		return v;
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> ceil(vector<C,D,O> v)
	{
		v.ceil();
		return v;
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> round(vector<C,D,O> v)
	{
		v.round();
		return v;
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> trunc(vector<C,D,O> v)
	{
		v.trunc();
		return v;
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	vector<C,D,O> abs(vector<C,D,O> v)
	{
		v.abs();
		return v;
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	C magnitude(const vector<C,D,O>& v)
	{
		return v.magnitude();
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	C quadrance(const vector<C,D,O>& v)
	{
		return v.quadrance();
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	C length(const vector<C,D,O>& v)
	{
		return v.length();
	}

	template <typename C, size_t D, typename O>
	[[nodiscard]]
	constexpr
	auto signum(vector<C,D,O> v)
	{
		v.signum();
		return v;
	}

	// for ADL to find these
	using ::operator~;
	using ::operator+;
	using ::operator-;
	using ::operator*;
	using ::operator/;
	using ::operator%;
	using ::operator&;
	using ::operator|;
	using ::operator^;
	using ::operator<<;
	using ::operator>>;
	using ::operator+=;
	using ::operator-=;
	using ::operator*=;
	using ::operator/=;
	using ::operator%=;
	using ::operator&=;
	using ::operator|=;
	using ::operator^=;
	using ::operator<<=;
	using ::operator>>=;

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
			out << "^";
		out << "\n";

		for(size_t i = 0; i < M; ++i)
			out << vector[i] << "\n";

		for(size_t i = 0; i < N; ++i)
			out << "v";
		out << "\n";

		return out;
	}

	template <typename F, typename... R> vector(F firts, R... rest)
		-> vector<F, sizeof...(R) + 1>;

	// ugh, clang is really nitpickey about default template arguments
	// actually, might be this bug
	// https://stackoverflow.com/questions/57133186/g-and-clang-different-behaviour-when-stdmake-index-sequence-and-stdin
	// very annoying in many different places :/
	template <typename C, size_t D, typename O, void* SFINAE> vector(vector<C,D,O,SFINAE>)
		-> vector<vector<C,D,O,SFINAE>,1>;

	template <typename>
	struct is_vector_instance : public std::false_type {};

	template <typename C, size_t D, typename O>
	struct is_vector_instance<vector<C,D,O>> : public std::true_type {};

	template <typename V>
	constexpr auto is_vector_instance_v = is_vector_instance<V>::value;

	template <size_t I, typename V,
		std::enable_if_t<is_vector_instance_v<std::decay_t<V>>>* = nullptr>
	constexpr decltype(auto) get(V&& v)
	{ return std::forward<V>(v).template get<I>(); }

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

	[[nodiscard]]
	constexpr static vec min()
	{
		static_assert(limits::is_specialized);
		vec m{};
		for(auto&& c : m)
			c = limits::min();
		return m;
	}

	[[nodiscard]]
	constexpr static vec lowest()
	{
		static_assert(limits::is_specialized);
		vec m{};
		for(auto&& c : m)
			c = limits::lowest();
		return m;
	}

	[[nodiscard]]
	constexpr static vec max()
	{
		static_assert(limits::is_specialized);
		vec m{};
		for(auto&& c : m)
			c = limits::max();
		return m;
	}
};

template <typename C, size_t D, typename O>
struct std::tuple_size<simple::geom::vector<C,D,O>> :
	std::integral_constant<size_t, D> {};

template <size_t I, typename C, size_t D, typename O>
struct std::tuple_element<I, simple::geom::vector<C,D,O>>
{
	using type = C;
};

#include "bool_algebra.hpp" // oof TODO: comparison ops are here now, though some would say not having them is a blessing, that would be a breaking change...

#endif /* end of include guard */
