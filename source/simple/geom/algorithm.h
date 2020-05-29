#ifndef SIMPLE_GEOM_ALGORITHM_H
#define SIMPLE_GEOM_ALGORITHM_H
#include "vector.hpp"
#include "simple/support/range.hpp"

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

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function, size_t LoopDimesntions = Dimensions>
	constexpr void loop_on
	(
		vector<Coordinate, Dimensions, Order>& index,
		const support::range<vector<Coordinate, Dimensions, Order>>&,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	);

	template <typename Coordinate, size_t Dimensions, typename Order, typename Function>
	constexpr void loop
	(
		const support::range<vector<Coordinate, Dimensions, Order>>&,
		const vector<Coordinate, Dimensions, Order>& step,
		Function&& callback
	);

	template <typename Coordinate,
		size_t Rows, size_t Columns,
		typename RowOrder, typename ColumnOrder
	>
	constexpr auto gauss_jordan_elimination
	(
		vector<
			vector<Coordinate, Columns, ColumnOrder>,
			Rows, RowOrder
		>
	);


	template <typename Transform, typename Vector,
		typename AnotherCoord = std::invoke_result_t<Transform,
			typename Vector::meta::coordinate_type>,
		typename Result = typename Vector::
			template map_coordinate_t<AnotherCoord>
	>
	[[nodiscard]] constexpr Result
	deep_mutant_clone(const Vector&, Transform&&);

} // namespace simple::geom

#endif /* end of include guard */
