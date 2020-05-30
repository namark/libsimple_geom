#include "simple/geom/vector.hpp"

using simple::geom::vector;

vector OO(false,false);
vector OI(false,true);
vector IO(true,false);
vector II(true,true);

auto c_OO = to_conjunction(OO); auto d_OO = to_disjunction(OO);
auto c_OI = to_conjunction(OI); auto d_OI = to_disjunction(OI);
auto c_IO = to_conjunction(IO); auto d_IO = to_disjunction(IO);
auto c_II = to_conjunction(II); auto d_II = to_disjunction(II);

void UntruthTable()
{
	assert( ! c_OO ); assert( ! d_OO );
	assert( ! c_OI ); assert(   d_OI );
	assert( ! c_IO ); assert(   d_IO );
	assert(   c_II ); assert(   d_II );
}

void DeMorgan()
{
	assert( ~c_OO == d_II ); assert( ~d_OO == c_II );
	assert( ~c_OI == d_IO ); assert( ~d_OI == c_IO );
	assert( ~c_IO == d_OI ); assert( ~d_IO == c_OI );
	assert( ~c_II == d_OO ); assert( ~d_II == c_OO );
}

void ReducingOperators()
{
	assert( (d_OO || d_OO) == false); assert( (c_OO || c_OO) == false);
	assert( (d_OO || d_OI) == true ); assert( (c_OO || c_OI) == false);
	assert( (d_OO || d_IO) == true ); assert( (c_OO || c_IO) == false);
	assert( (d_OO || d_II) == true ); assert( (c_OO || c_II) == true );
	assert( (d_OI || d_OI) == true ); assert( (c_OI || c_OI) == false);
	assert( (d_OI || d_IO) == true ); assert( (c_OI || c_IO) == false);
	assert( (d_OI || d_II) == true ); assert( (c_OI || c_II) == true );
	assert( (d_IO || d_IO) == true ); assert( (c_IO || c_IO) == false);
	assert( (d_IO || d_II) == true ); assert( (c_IO || c_II) == true );
	assert( (d_II || d_II) == true ); assert( (c_II || c_II) == true );

	assert( (d_OO && d_OO) == false); assert( (c_OO && c_OO) == false);
	assert( (d_OO && d_OI) == false); assert( (c_OO && c_OI) == false);
	assert( (d_OO && d_IO) == false); assert( (c_OO && c_IO) == false);
	assert( (d_OO && d_II) == false); assert( (c_OO && c_II) == false);
	assert( (d_OI && d_OI) == true ); assert( (c_OI && c_OI) == false);
	assert( (d_OI && d_IO) == true ); assert( (c_OI && c_IO) == false);
	assert( (d_OI && d_II) == true ); assert( (c_OI && c_II) == false);
	assert( (d_IO && d_IO) == true ); assert( (c_IO && c_IO) == false);
	assert( (d_IO && d_II) == true ); assert( (c_IO && c_II) == false);
	assert( (d_II && d_II) == true ); assert( (c_II && c_II) == true );
}

void ElementwiseOperators()
{
	assert( (d_OO | d_OO) == OO); assert( (c_OO | c_OO) == OO);
	assert( (d_OO | d_OI) == OI); assert( (c_OO | c_OI) == OI);
	assert( (d_OO | d_IO) == IO); assert( (c_OO | c_IO) == IO);
	assert( (d_OO | d_II) == II); assert( (c_OO | c_II) == II);
	assert( (d_OI | d_OI) == OI); assert( (c_OI | c_OI) == OI);
	assert( (d_OI | d_IO) == II); assert( (c_OI | c_IO) == II);
	assert( (d_OI | d_II) == II); assert( (c_OI | c_II) == II);
	assert( (d_IO | d_IO) == IO); assert( (c_IO | c_IO) == IO);
	assert( (d_IO | d_II) == II); assert( (c_IO | c_II) == II);
	assert( (d_II | d_II) == II); assert( (c_II | c_II) == II);

	assert( (d_OO & d_OO) == OO); assert( (c_OO & c_OO) == OO);
	assert( (d_OO & d_OI) == OO); assert( (c_OO & c_OI) == OO);
	assert( (d_OO & d_IO) == OO); assert( (c_OO & c_IO) == OO);
	assert( (d_OO & d_II) == OO); assert( (c_OO & c_II) == OO);
	assert( (d_OI & d_OI) == OI); assert( (c_OI & c_OI) == OI);
	assert( (d_OI & d_IO) == OO); assert( (c_OI & c_IO) == OO);
	assert( (d_OI & d_II) == OI); assert( (c_OI & c_II) == OI);
	assert( (d_IO & d_IO) == IO); assert( (c_IO & c_IO) == IO);
	assert( (d_IO & d_II) == IO); assert( (c_IO & c_II) == IO);
	assert( (d_II & d_II) == II); assert( (c_II & c_II) == II);
}

auto OO_OO = vector(OO,OO); auto OI_OO = vector(OI,OO);
auto OO_OI = vector(OO,OI); auto OI_OI = vector(OI,OI);
auto OO_IO = vector(OO,IO); auto OI_IO = vector(OI,IO);
auto OO_II = vector(OO,II); auto OI_II = vector(OI,II);

auto IO_OO = vector(IO,OO); auto II_OO = vector(II,OO);
auto IO_OI = vector(IO,OI); auto II_OI = vector(II,OI);
auto IO_IO = vector(IO,IO); auto II_IO = vector(II,IO);
auto IO_II = vector(IO,II); auto II_II = vector(II,II);

void ExpandingOperators()
{
	assert( (d_OO * d_OO) == to_disjunction(OO_OO) );
	assert( (d_OO * d_OI) == to_disjunction(OO_OO) );
	assert( (d_OO * d_IO) == to_disjunction(OO_OO) );
	assert( (d_OO * d_II) == to_disjunction(OO_OO) );
	assert( (d_OI * d_OO) == to_disjunction(OO_OO) );
	assert( (d_OI * d_OI) == to_disjunction(OO_OI) );
	assert( (d_OI * d_IO) == to_disjunction(OI_OO) );
	assert( (d_OI * d_II) == to_disjunction(OI_OI) );
	assert( (d_IO * d_OO) == to_disjunction(OO_OO) );
	assert( (d_IO * d_OI) == to_disjunction(OO_IO) );
	assert( (d_IO * d_IO) == to_disjunction(IO_OO) );
	assert( (d_IO * d_II) == to_disjunction(IO_IO) );
	assert( (d_II * d_OO) == to_disjunction(OO_OO) );
	assert( (d_II * d_OI) == to_disjunction(OO_II) );
	assert( (d_II * d_IO) == to_disjunction(II_OO) );
	assert( (d_II * d_II) == to_disjunction(II_II) );

	assert( (c_OO + c_OO) == to_conjunction(OO_OO) );
	assert( (c_OO + c_OI) == to_conjunction(OO_II) );
	assert( (c_OO + c_IO) == to_conjunction(II_OO) );
	assert( (c_OO + c_II) == to_conjunction(II_II) );
	assert( (c_OI + c_OO) == to_conjunction(OI_OI) );
	assert( (c_OI + c_OI) == to_conjunction(OI_II) );
	assert( (c_OI + c_IO) == to_conjunction(II_OI) );
	assert( (c_OI + c_II) == to_conjunction(II_II) );
	assert( (c_IO + c_OO) == to_conjunction(IO_IO) );
	assert( (c_IO + c_OI) == to_conjunction(IO_II) );
	assert( (c_IO + c_IO) == to_conjunction(II_IO) );
	assert( (c_IO + c_II) == to_conjunction(II_II) );
	assert( (c_II + c_OO) == to_conjunction(II_II) );
	assert( (c_II + c_OI) == to_conjunction(II_II) );
	assert( (c_II + c_IO) == to_conjunction(II_II) );
	assert( (c_II + c_II) == to_conjunction(II_II) );
}

int main()
{
	UntruthTable();
	DeMorgan();
	ReducingOperators();
	ElementwiseOperators();
	ExpandingOperators();
	return 0;
}
