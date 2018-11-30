#ifndef SIMPLE_GEOM_BOX_HPP
#define SIMPLE_GEOM_BOX_HPP

#include "vector.hpp"
#include "simple/support/range.hpp"

namespace simple::geom
{

	template <typename Type>
	struct segment
	{
		Type size, position;

		using range = support::range<Type>;

		constexpr operator range() const
		{
			return range{position, position + size};
		}
	};

	template <typename Type, typename AnchorType = Type>
	struct anchored_segment : public segment<Type>
	{
		using base = segment<Type>;
		AnchorType anchor;

		constexpr anchored_segment() = default;

		constexpr operator typename base::range() const
		{
			auto lower = this->position -
				static_cast<Type>(anchor * static_cast<AnchorType>(this->size));
			return typename base::range{lower, lower + this->size};
		}

	};

	template <typename Coordinate, size_t Dimensions = 2>
	using vector_segment = segment<vector<Coordinate, Dimensions>>;

	template <typename Coordinate, size_t Dimensions = 2, typename Anchor = Coordinate>
	using anchored_vector_segment = anchored_segment<vector<Coordinate, Dimensions>, vector<Anchor, Dimensions>>;

} // namespace simple::geom

#endif /* end of include guard */
