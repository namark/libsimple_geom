#ifndef SIMPLE_GEOM_ALGORITHM_HPP
#define SIMPLE_GEOM_ALGORITHM_HPP
#include "algorithm.h"

#include <functional>

#include "simple/support/algorithm.hpp"

namespace simple::geom
{

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function, size_t LoopDimesntions>
	constexpr void loop_on
	(
		vector<Coordinate, Dimensions, Order>& index,
		const vector<Coordinate, Dimensions, Order>& lower,
		const vector<Coordinate, Dimensions, Order>& upper,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	)
	{
		if constexpr (LoopDimesntions == 0)
			std::invoke(std::forward<Function>(callback),index);
		else
		{
			constexpr size_t I = LoopDimesntions-1;
			for
			(
				index[I] = lower[I];
				index[I] < upper[I];
				index[I] += step[I]
			)
			{
				loop_on<Coordinate,Dimensions,Order,Function,I>
					(index, lower, upper, step,
					 std::forward<Function>(callback));
			}
		}
	};

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function>
	constexpr void loop
	(
		const vector<Coordinate, Dimensions, Order>& lower,
		const vector<Coordinate, Dimensions, Order>& upper,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	)
	{
		vector<Coordinate, Dimensions, Order> index{};
		loop_on(index, lower, upper, step,
			 std::forward<Function>(callback));
	}

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function>
	constexpr void loop
	(
		const vector<Coordinate, Dimensions, Order>& upper,
		Function&& callback
	)
	{
		using vector = vector<Coordinate, Dimensions, Order>;
		vector index{};
		loop_on(index, vector::zero(),  upper, vector::one(),
			 std::forward<Function>(callback));
	}

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function, size_t LoopDimesntions>
	constexpr void loop_on
	(
		vector<Coordinate, Dimensions, Order>& index,
		const support::range<vector<Coordinate, Dimensions, Order>>& bounds,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	)
	{
		loop_on(index, bounds.lower(), bounds.upper(), step,
			 std::forward<Function>(callback));
	}

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function>
	constexpr void loop
	(
		const support::range<vector<Coordinate, Dimensions, Order>>& bounds,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	)
	{
		vector<Coordinate, Dimensions, Order> index{};
		loop_on(index, bounds, step,
			 std::forward<Function>(callback));
	}

	template <typename Coordinate,
		size_t Rows, size_t Columns,
		typename RowOrder, typename ColumnOrder
	>
	constexpr auto gauss_jordan_elimination
	(
		vector<
			vector<Coordinate, Columns, ColumnOrder>,
			Rows, RowOrder
		> m
	)
	{
		constexpr size_t D = std::min(Rows, Columns);
		for( size_t i = 0; i < D; ++i)
		{
			auto pivot = std::max_element(m.begin()+i, m.end(), [&i](auto a, auto b)
			{
				using std::abs;
				return abs(a[i]) < abs(b[i]);
			});
			support::swap(*pivot, m[i]);
			m[i] /= +m[i][i]; // ouch, easy to shoot foot here
			for(size_t j = i+1; j < D; ++j)
				m[j] -=	m[i] * m[j][i];
			for(size_t j = i; j --> 0;)
				m[j] -=	m[i] * m[j][i];
		}
		return m;
	}

	template <typename Transform, typename Vector, typename AnotherCoord, typename Result>
	[[nodiscard]] constexpr Result
	deep_mutant_clone(const Vector& vector, Transform&& transform)
	{
		Result result{};
		loop(Vector::meta::template dimensions<>, [&](auto i)
		{
			// std::invoke is not constexpr -_-
			result[i] = std::apply(
				std::forward<Transform>(transform),
				std::forward_as_tuple(vector[i])
			);
		});
		return result;
	}


} // namespace simple::geom

#endif /* end of include guard */
