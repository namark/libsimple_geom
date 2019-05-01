#include <cassert>
#include "simple/geom/segment.hpp"
using namespace simple::geom;

using int2 = vector<int, 2>;
using float2 = vector<float, 2>;
using rect = segment<int2>;
using anchored_rect = anchored_segment<int2, float2>;

void Initialization()
{
	[[maybe_unused]] rect garbage;
	[[maybe_unused]] anchored_rect garbage2;

	// default
	{
		rect r{};
		assert(r.position == int2{});
		assert(r.size == int2{});

		anchored_rect ar{};
		assert(ar.position == int2{});
		assert(ar.size == int2{});
		assert(ar.anchor == float2{});
	}

	// size
	{

		rect r{{10,10}};
		assert(r.position == int2{});
		assert(( r.size == int2{10,10} ));

		rect r2{r.size};
		assert(r2.position == int2{});
		assert(( r2.size == r.size ));

		anchored_rect ar{ {{10,10}} };
		assert(ar.position == int2{});
		assert(( ar.size == int2{10,10} ));
		assert(ar.anchor == float2{});

		anchored_rect ar2{ {ar.size} };
		assert(ar2.position == int2{});
		assert(( ar2.size == ar.size ));
		assert(ar2.anchor == float2{});

	}

	// size position
	{

		rect r{{10,10}, {11,11}};
		assert(( r.position == int2{11,11} ));
		assert(( r.size == int2{10,10} ));

		rect r2{r.size, r.position};
		assert(( r2.position == r.position ));
		assert(( r2.size == r.size ));

		anchored_rect ar{ {{10,10}, {11,11}} };
		assert(( ar.position == int2{11,11} ));
		assert(( ar.size == int2{10,10} ));
		assert(ar.anchor == float2{});

		anchored_rect ar2{ {ar.size, ar.position} };
		assert(( ar2.position == ar.position ));
		assert(( ar2.size == ar.size ));
		assert(ar2.anchor == float2{});

		anchored_rect ar3{ r };
		assert(( ar3.position == r.position ));
		assert(( ar3.size == r.size ));
		assert(ar3.anchor == float2{});

	}

	// size anchor
	{
		anchored_rect ar{ {{10,10}}, float2::one(0.5f) };
		assert(( ar.position == int2{} ));
		assert(( ar.size == int2{10,10} ));
		assert(ar.anchor == float2::one(0.5f));

		anchored_rect ar2{ {ar.size}, float2::one(0.5f) };
		assert(( ar2.position == int2{} ));
		assert(( ar2.size == ar.size ));
		assert(ar2.anchor == float2::one(0.5f));

		anchored_rect ar3{ ar.size, {}, float2::one(0.5f) };
		assert(( ar3.position == int2{} ));
		assert(( ar3.size == ar.size ));
		assert(ar3.anchor == float2::one(0.5f));
	}

	// size position anchor
	{
		rect r{{10,10}, {11,11}};

		anchored_rect ar{ {{10,10}, {11,11}}, float2::one()};

		anchored_rect ar2 { ar.size, ar.position, ar.anchor};
		assert(( ar2.position == ar.position ));
		assert(( ar2.size == ar.size ));
		assert(( ar2.anchor == ar.anchor ));

		anchored_rect ar3{ r, ar.anchor };
		assert(( ar3.position == r.position ));
		assert(( ar3.size == r.size ));
		assert(( ar3.anchor == ar.anchor ));

		anchored_rect ar4{ ar, float2::one(0.5f) };
		assert(( ar4.position == ar.position ));
		assert(( ar4.size == ar.size ));
		assert(( ar4.anchor == float2::one(0.5f) ));
		assert(( ar4.anchor != ar.anchor ));

	}

}

void Conversions()
{
	//TODO
	auto a = anchored_rect::range{};
	const anchored_rect r{};
	a = r;
}

int main()
{

	return 0;
}
