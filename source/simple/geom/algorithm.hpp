#ifndef SIMPLE_GEOM_ALGORITHM_HPP
#define SIMPLE_GEOM_ALGORITHM_HPP
#include "algorithm.h"

#include <functional>

namespace simple::geom
{

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function, size_t LoopDimesntions = Dimensions>
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

} // namespace simple::geom

#endif /* end of include guard */
