#include <cassert>
#include "simple/support/range.hpp"
#include "simple/geom/vector.hpp"

using namespace simple;
using namespace geom;
using namespace support;

int main()
{
	// just making sure this compiles... very paranoid
	void(range{vector(10,20), vector(20,30)}.limit());

	#include "range_intersection.data"
	return 0;
}
