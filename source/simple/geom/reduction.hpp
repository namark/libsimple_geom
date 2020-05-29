#ifndef SIMPLE_GEOM_REDUCTION_HPP
#define SIMPLE_GEOM_REDUCTION_HPP

#include <utility>
#include "simple/support/type_traits.hpp"

namespace simple::geom
{

	template <typename Range>
	using range_value_t = support::remove_cvref_t<
		decltype(*std::declval<Range>().begin())
	>;

	// TODO: in_place version that accepts value by reference?
	template <typename Op, typename Value, typename Range,
		std::enable_if_t<
			std::is_same_v<range_value_t<Range>, Value>
		>* = nullptr>
	constexpr Value deep_reduce(
		const Range& range, Value init = Op::identity )
	{
		for(auto&& one : range)
			init = Op{}(init, one);
		return init;
	}

	template <typename Op, typename Value, typename Range,
		std::enable_if_t<
			!std::is_same_v<range_value_t<Range>, Value>
			// is_range_v<range_value_t<Range>>
		>* = nullptr>
	constexpr Value deep_reduce(
		const Range& range, Value init = Op::identity )
	{
		for(auto&& one : range)
			init = deep_reduce<Op>(one, init);
		return init;
	}

	// NOTE: Not really simple::geom specific, make sense with any range
	template <typename Operation, typename Range,
		typename Value = range_value_t<Range> >
	struct reduction {
		using value_type = Value;
		using range_type = Range;
		using operation = Operation;

		constexpr static auto identity = Operation::identity;

		Range range;

		constexpr explicit reduction(Range range) :
			range(std::move(range)) {}

		constexpr operator Range() const { return range; }
		template <typename Other, typename R = Range,
			std::enable_if_t<
				std::is_constructible_v<Other,R> &&
				not support::is_template_instance_v<
					simple::geom::reduction, Other
				>
			> * = nullptr
		>
		constexpr operator Other() const { return static_cast<Other>(range); }

		constexpr explicit operator value_type() const
		{
			return deep_reduce<operation>(range, identity);
		}

		[[nodiscard]]
		constexpr auto operator ==(const reduction& other) const noexcept
		{
			return range == other.range;
		}

		[[nodiscard]]
		constexpr auto operator !=(const reduction& other) const noexcept
		{
			return range != other.range;
		}

	};

} // namespace simple::geom

#endif /* end of include guard */
