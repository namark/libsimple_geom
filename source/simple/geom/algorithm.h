#ifndef SIMPLE_GEOM_ALGORITHM_H
#define SIMPLE_GEOM_ALGORITHM_H
#include "vector.hpp"

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
	);

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function>
	constexpr void loop
	(
		const vector<Coordinate, Dimensions, Order>& lower,
		const vector<Coordinate, Dimensions, Order>& upper,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	);
} // namespace simple::geom

#endif /* end of include guard */
