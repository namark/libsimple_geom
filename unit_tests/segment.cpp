#include <cassert>
#include "simple/geom/segment.hpp"
using namespace simple::geom;

using int2 = vector<int>;
using rect = segment<int2>;
using anchored_rect = anchored_segment<int2, float>;

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
		assert(ar.anchor == float{});
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
		assert(ar.anchor == float{});

		anchored_rect ar2{ {ar.size} };
		assert(ar2.position == int2{});
		assert(( ar2.size == ar.size ));
		assert(ar2.anchor == float{});

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
		assert(ar.anchor == float{});

		anchored_rect ar2{ {ar.size, ar.position} };
		assert(( ar2.position == ar.position ));
		assert(( ar2.size == ar.size ));
		assert(ar2.anchor == float{});

		anchored_rect ar3{ r };
		assert(( ar3.position == r.position ));
		assert(( ar3.size == r.size ));
		assert(ar3.anchor == float{});

	}

	// size anchor
	{
		anchored_rect ar{ {{10,10}}, 0.5f };
		assert(( ar.position == int2{} ));
		assert(( ar.size == int2{10,10} ));
		assert(ar.anchor == 0.5f);

		anchored_rect ar2{ {ar.size}, 0.5 };
		assert(( ar2.position == int2{} ));
		assert(( ar2.size == ar.size ));
		assert(ar2.anchor == 0.5);

		anchored_rect ar3{ ar.size, {}, 0.5 };
		assert(( ar3.position == int2{} ));
		assert(( ar3.size == ar.size ));
		assert(ar3.anchor == 0.5);
	}

	// size position anchor
	{
		rect r{{10,10}, {11,11}};

		anchored_rect ar{ {{10,10}, {11,11}}, 1};

		anchored_rect ar2 { ar.size, ar.position, ar.anchor};
		assert(( ar2.position == ar.position ));
		assert(( ar2.size == ar.size ));
		assert(( ar2.anchor == ar.anchor ));

		anchored_rect ar3{ r, ar.anchor };
		assert(( ar3.position == r.position ));
		assert(( ar3.size == r.size ));
		assert(( ar3.anchor == ar.anchor ));

		anchored_rect ar4{ ar, 0.5 };
		assert(( ar4.position == ar.position ));
		assert(( ar4.size == ar.size ));
		assert(( ar4.anchor == 0.5 ));
		assert(( ar4.anchor != ar.anchor ));

	}

}

void Conversions()
{
	//TODO
}

int main()
{

	return 0;
}
