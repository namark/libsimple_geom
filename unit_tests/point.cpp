#include "simple/geom/vector.hpp"
#include <cmath>
#include <cassert>
#include <cstdio>

using namespace simple;
using geom::vector;
using float4 = vector<float, 4>;
using int4 = vector<int, 4>;


void ZeroConstruction()
{
	float4::array control{};
	float4 p{};
	float4 p2 = {};
	float4 p3 = float4();
	assert(control == static_cast<const float4::array>(p));
	assert(control == static_cast<const float4::array>(p2));
	assert(control == static_cast<const float4::array>(p3));
	assert(control == static_cast<const float4::array>(float4::zero()));

	// NOTE: can't really test for random garbage since random garbage can be anything, including all zeros
	// also UB, great -_-
	bool anyGarbage = false;
	std::array<float4, 100> garbagePile;
	for (auto& garbage : garbagePile)
	{
		anyGarbage |= (control != static_cast<float4::array>(garbage));
		if(anyGarbage) break;
	}
	assert(anyGarbage);

}

void EqualityComparison()
{
	int4 p(1, 2, 3, 4);
	int4 p2(1, 2, 3, 4);
	int4 p3(4, 3, 2, 1);
	assert(p == p2);
	assert(p != p3);
	assert( vector(1,2,3,4,5) != vector(0,0,0,0,5) ); // at some point this wasn't the case, and no other test caught it -_-
	assert( !(vector(1,2,3,4,5) == vector(0,0,0,0,5)) );
	assert( ~(vector(1,2,3,4,5) == vector(0,0,0,0,5)) );
}

void OtherConstruction()
{
	float4 p(1.0f, 2.0f, 3.0f, 4.0f);
	float4 p2{1.0f, 2.0f, 3.0f, 4.0f};
	float4 p3{{1.0f, 2.0f, 3.0f, 4.0f}};
	float4 p4{float4::array{1.0f, 2.0f, 3.0f, 4.0f}};
	float4 p5(float4::array{1.0f, 2.0f, 3.0f, 4.0f});
	float4 p6 = float4(int4(1, 2, 3, 4));
	float4 p7 {{1, 2, 3, 4}};
	float4 p8 (p7);
	float4 p9 = {p8};
	assert(p == p2);
	assert(p2 == p3);
	assert(p3 == p4);
	assert(p4 == p5);
	assert(p5 == p6);
	assert(p6 == p7);
	assert(p7 == p8);
	assert(p8 == p9);
	assert(p9 != float4::zero());
	static_assert(int4(1,1,1,1) == int4::one(), "");
	static_assert(int4(4,4,4,4) == int4::one(4), "");
	static_assert(float4(1.0f,1.0f,1.0f,1.0f) == float4::one(), "");
	static_assert(int4(1,0,0,0) == int4::i(), "");
	static_assert(int4(0,1,0,0) == int4::j(), "");
	static_assert(int4(0,0,1,0) == int4::k(), "");
	static_assert(int4(0,0,0,1) == int4::l(), "");
	static_assert(int4(4,0,0,0) == int4::i(4), "");
	static_assert(int4(0,4,0,0) == int4::j(4), "");
	static_assert(int4(0,0,4,0) == int4::k(4), "");
	static_assert(int4(0,0,0,4) == int4::l(4), "");
	static_assert(int4(-1,1,-1,-1) == int4::unit<1>(1,-1), "");
}

void Mutation()
{
	float4 p {1.3f, 1.5f, 2.1f, 4.6f};
	int4 p2 {1, 2, 2, 5};
	int4 p3 {1, 1, 2, 4};
	assert( p2 == p.mutant_clone( [](auto x) -> int { return std::round(x); } ));
	assert( p3 == p.mutant_clone( [](auto x) { return int(x); } ));
}

void Mixing()
{
	int4 p {1, 2, 3, 4};
	assert( (geom::vector<int, 2>{1,2} == p.mix<0,1>()) );
	assert( (geom::vector<int, 2>{1,2} == p.mix<2>({0,1})) );
	assert( (geom::vector<int, 3>{2,3,4} == p.mix<1,2,3>()) );
	assert( (geom::vector<int, 3>{2,3,4} == p.mix<3>({1,2,3})) );
	assert( (geom::vector<int, 6>{3,3,3,2,2,1} == p.mix<2,2,2,1,1,0>()) );
	assert( (geom::vector<int, 6>{3,3,3,2,2,1} == p.mix<6>({2,2,2,1,1,0})) );
	assert( (geom::vector<int, 3>{1,2,0} == p.mix<0,1,4>(0)) );
	assert( (geom::vector<int, 3>{1,2,0} == p.mix<3>({0,1,4}, 0)) );

	assert( (vector(1,2) == p.first<2>()) );
	assert( (vector(2,3,4) == p.last<3>()) );
}

void MultidimensionalElementAccess()
{

	auto spiral_walk = [](auto room)
	{
		constexpr auto csize = decltype(room)::meta::template dimensions<int>;
		vector <
			std::remove_reference_t<decltype(room[csize-1])>,
			csize.x()*csize.y()
		> ret{};

		auto size = csize;
		size_t i = 0;

		auto direction = 1;
		auto begin = vector(-1,0);
		while(size > vector(0,0))
		{
			auto end = begin + direction * (size - vector(0,1)) ;

			do
			{
				begin.x() += direction;
				ret[i++] = room[begin];
			}
			while(begin.x() != end.x());
			while(begin.y() != end.y())
			{
				begin.y() += direction;
				ret[i++] = room[begin];
			}
			//do

			--size;
			direction = -direction;
		}

		return ret;
	};

	assert( spiral_walk(vector(
		vector(1,2,3),
		vector(4,5,6),
		vector(7,8,9)
	)) == vector(1,2,3,6,9,8,7,4,5));

	assert( spiral_walk(vector(
		vector(1),
		vector(2)
	)) == vector(1,2));

	assert( spiral_walk(vector(
		vector(1)
	)) == vector(1));

	assert( spiral_walk(vector(
		vector(1,2)
	)) == vector(1,2));

	// // well clang really doesn't like zero size vectors so...
	// assert( spiral_walk(vector(
	// 	vector<int,0>()
	// )) == (vector<int,0>{}));
    //
	// assert( spiral_walk(vector(
	// 	vector<int,0>(),
	// 	vector<int,0>(),
	// 	vector<int,0>(),
	// 	vector<int,0>()
	// )) == (vector<int,0>{}));
	//
	// assert( spiral_walk(vector<vector<int,10>,0>()
	// ) == (vector<int,0>{}));

	assert( spiral_walk(vector(
		vector(1,2),
		vector(3,4)
	)) == vector(1,2,4,3));

	assert( spiral_walk(vector(
		vector( 1, 2, 3, 4),
		vector( 5, 6, 7, 8),
		vector( 9,10,11,12)
	)) == vector(1,2,3,4,8,12,11,10,9,5,6,7));

	assert( spiral_walk(vector(
		vector( 1, 2, 3),
		vector( 4, 5, 6),
		vector( 7, 8, 9),
		vector(10,11,12)
	)) == vector(1,2,3,6,9,12,11,10,7,4,5,8));

	assert( spiral_walk(vector(
		vector( 1, 2, 3, 4),
		vector( 5, 6, 7, 8),
		vector( 9,10,11,12),
		vector(13,14,15,16)
	)) == vector(1,2,3,4,8,12,16,15,14,13,9,5,6,7,11,10));

}

void RangeBasedLooping()
{
	const float4 p{1.0f, 2.0f, 3.0f, 4.0f};
	float4 p2;

	int num = 1;
	for(auto& coord : p2)
	{
		coord = num;
		num += 1;
	}
	assert(p == p2);

	num = 1;
	for(auto coord : p)
	{
		assert(coord == num);
		num += 1;
	}
}

void Arithmetic()
{
	float4 p{{1, 2, 3, 4}};
	float4 p2 = p;
	float4 sum{{2, 4, 6, 8}};
	float4 inc{{2, 3, 4, 5}};
	float4 dec{{0, 1, 2, 3}};
	assert(p + p2 == sum);
	assert(p + 1 == inc);
	assert(1 + p == inc);
	assert(p - 1 == dec);
	assert(p - dec == float4::one());
	assert(p - p2 == float4::zero());
	assert(-p == float4(int4(-1, -2, -3, -4)));

	assert(p2++ == p);
	assert(p2 == p+1);

	p2 = p;
	assert(++p2 == p+1);
	assert(p2 == p+1);

	p2 = p;
	assert(p2-- == p);
	assert(p2 == p-1);

	p2 = p;
	assert(--p2 == p-1);
	assert(p2 == p-1);

	p2 = p;
	p2 += p;
	assert(p2 == p+p);
	p2 -= p;
	assert(p2 == p);
	p2 -= p;
	assert(p2 == float4::zero());

	p2 = p;
	p2 += 3;
	assert(p2 == p + 3);
	p2 -= 1.5f;
	assert(p2 == p + 1.5f);

	assert( p + p == p * 2);
	assert( p * p == float4(1.0f, 4.0f, 9.0f, 16.0f));

	assert( (p + p)/2 == p);
	assert( p / (float4::one() * 2) == float4(0.5f, 1.0f, 1.5f, 2.0f) );
	assert( p / p == float4::one());

	p2 = p;
	p2 *= 2;
	assert(p2 == p*2);
	p2 *= p;
	assert(p2 == p*p*2);
	p2 /= p;
	assert(p2 == p*2);
	p2 /= 2;
	assert(p2 == p);

	p = {1.f, 2.f, 3.f, 4.f};
	p2 = {1.f, 0.f, 1.f, 0.f};
	float4 p3 = {1.f, 2.f, 0.f, 1.f};
	assert(p%2.f == p2);
	assert(p%float4::one(2.f) == p2);
	p %= 3.f;
	assert(p == p3);

	p = {1.f, 2.f, 3.f, 4.f};
	p3 = {4.f, 5.f, 7.f, 9.f};
	p2 = {3.f, 3.f, 4.f, 5.f};
	assert(p3 % p2 == p);
	p3 %= p2;
	assert(p3 == p);

}

// discrete? what?
// TODO: used unsigned and if constexpr guard implied sizes
void DiscreteArithmetic()
{
	// %
	int4 p(1, 2, 3, 4);
	int4 p2(1, 0, 1, 0);
	int4 p3(1, 2, 0, 1);
	assert(p%2 == p2);
	p %= 3;
	assert(p == p3);

	p = {1, 2, 3, 4};
	p3 = {4, 5, 7, 9};
	p2 = {3, 3, 4, 5};
	assert(p3 % p2 == p);
	p3 %= p2;
	assert(p3 == p);

	// <<
	p = {0xA, 0xB0, 0xC00, 0xD000};
	p2 = {0xA0, 0xB00, 0xC000, 0xD0000};
	assert((p << 4) == p2);
	p <<= 4;
	assert(p == p2);

	p = {0xA, 0xB0, 0xC00, 0xD000};
	p2 = {0xA, 0xB, 0xC, 0xD};
	p3 = {0, 1, 2, 3};
	assert((p2 << (p3*4)) == p);
	p2 <<= (p3*4);
	assert(p2 == p);

	// >>
	p = {0xA, 0xB0, 0xC00, 0xD000};
	p2 = {0xA0, 0xB00, 0xC000, 0xD0000};
	assert((p2 >> 4) == p);
	p2 >>= 4;
	assert(p == p2);

	p = {0xA, 0xB0, 0xC00, 0xD000};
	p2 = {0xA, 0xB, 0xC, 0xD};
	p3 = {0, 1, 2, 3};
	assert((p >> (p3*4)) == p2);
	p >>= (p3*4);
	assert(p == p2);

	// &
	p =  {0b0101, 0b1010, 0b1111, 0b0000};
	p2 = {0b0101, 0b0000, 0b0101, 0b0000};
	assert((p & 0b101) == p2);
	p &= 0b0101;
	assert(p == p2);

	p =  {0b0101, 0b1010, 0b1111, 0b0000};
	p2 = {0b0110, 0b0111, 0b1001, 0b1111};
	p3 = {0b0100, 0b0010, 0b1001, 0b0000};
	assert((p & p2) == p3);
	p &= p2;
	assert(p == p3);

	// |
	p =  {0b0101, 0b1010, 0b1111, 0b0000};
	p2 = {0b0101, 0b1111, 0b1111, 0b0101};
	assert((p | 0b0101) == p2);
	p |= 0b0101;
	assert(p == p2);

	p =  {0b0101, 0b1010, 0b1111, 0b0000};
	p2 = {0b0110, 0b0111, 0b1001, 0b1111};
	p3 = {0b0111, 0b1111, 0b1111, 0b1111};
	assert((p | p2) == p3);
	p |= p2;
	assert(p == p3);

	// ^
	p =  {0b0101, 0b1010, 0b1111, 0b0000};
	p2 = {0b0000, 0b1111, 0b1010, 0b0101};
	assert((p ^ 0b0101) == p2);
	p ^= 0b0101;
	assert(p == p2);

	p =  {0b0101, 0b1010, 0b1111, 0b0000};
	p2 = {0b0110, 0b0111, 0b1001, 0b1111};
	p3 = {0b0011, 0b1101, 0b0110, 0b1111};
	assert((p ^ p2) == p3);
	p ^= p2;
	assert(p == p3);

	// ~
	p =                          {0b0101, 0b1010, 0b1111, 0b0000};
	assert( (~p & 0b1111) == int4(0b1010, 0b0101, 0b0000, 0b1111));

	// ~ for vector<bool>
	assert( ~vector(true, false, true, false) == vector(false, true, false, true) );
	assert( ~vector(true, true, false, false) == vector(false, false, true, true) );
	assert( ~vector(true, true, true, true) == vector(false, false, false, false) );

	// ! for vector<bool>
	assert( (!vector(true, false, true, false)) == vector(false, true, false, true) );
	assert( (!vector(true, true, false, false)) == vector(false, false, true, true) );
	assert( (!vector(true, true, true, true)) == vector(false, false, false, false) );

	// ~ for vector<vector<bool>>
	assert( (~vector(vector(true, false), vector(true, false))) ==
		vector(vector(false, true), vector(false, true)) );
	assert( (~vector(vector(true, true), vector(false, false))) ==
		vector(vector(false, false), vector(true, true)) );
	assert( (~vector(vector(true, true), vector(true, true))) ==
		vector(vector(false, false), vector(false, false)) );

	// ! for vector<vector<bool>>
	assert( (!vector(vector(true, false), vector(true, false))) ==
		vector(vector(false, true), vector(false, true)) );
	assert( (!vector(vector(true, true), vector(false, false))) ==
		vector(vector(false, false), vector(true, true)) );
	assert( (!vector(vector(true, true), vector(true, true))) ==
		vector(vector(false, false), vector(false, false)) );

}

void ComparisonOperators()
{
	int4 p(1, 2, 3, 4);
	int4 p2(1, -2, 0, 1);

	// some random masking logic
	assert(p * int4(p2 > int4::zero()) == int4(1, 0, 0, 4));
	assert(p * int4(p2 < int4::zero()) == int4(0, 2, 0, 0));
	assert(p * int4(p2 == int4::zero()) == int4(0, 0, 3, 0));
	assert(p * int4(p2 == int4::zero() | p2 == int4::one()) == int4(1, 0, 3, 4));
	assert(p * int4(p2 != int4::zero() | p2 == int4::one()) == int4(1, 2, 0, 4));
	assert(p * int4(p2 != int4::zero() & p2 != int4::one(-2)) == int4(1, 0, 0, 4));
}

int PotentiallyAmbigous(geom::vector<int, 2>){ return 2; }
int PotentiallyAmbigous(geom::vector<int, 3>){ return 3; }
void DisabiguateOnConstructorParameterCount()
{
	assert( PotentiallyAmbigous({1,2,3}) == 3 );
	assert( PotentiallyAmbigous({1,2}) == 2 );
}

void NumericLimits()
{
	using limits = std::numeric_limits<int4>;
	using coordLimits = std::numeric_limits<int4::coordinate_type>;
	static_assert(limits::is_specialized);
	static_assert(limits::max() == int4::one(coordLimits::max()));
	static_assert(limits::min() == int4::one(coordLimits::min()));
}

void CoordinateOrder()
{
	geom::vector<int, 4, std::index_sequence<3,2,1,0>> p_wzyx(1,2,3,4);
	assert( p_wzyx.x() == 4 );
	assert( p_wzyx.y() == 3 );
	assert( p_wzyx.z() == 2 );
	assert( p_wzyx.w() == 1 );
	assert( p_wzyx.get<0>() == 4 );
	assert( p_wzyx.get<1>() == 3 );
	assert( p_wzyx.get<2>() == 2 );
	assert( p_wzyx.get<3>() == 1 );
	assert( p_wzyx[p_wzyx.x_index] == 4 );
	assert( p_wzyx[p_wzyx.y_index] == 3 );
	assert( p_wzyx[p_wzyx.z_index] == 2 );
	assert( p_wzyx[p_wzyx.w_index] == 1 );
	assert( p_wzyx[0] == 1 );
	assert( p_wzyx[1] == 2 );
	assert( p_wzyx[2] == 3 );
	assert( p_wzyx[3] == 4 );
	assert( int4(p_wzyx) == int4(4,3,2,1) );

	auto [x,y,z,w] = p_wzyx;
	assert( x == p_wzyx.x() );
	assert( y == p_wzyx.y() );
	assert( z == p_wzyx.z() );
	assert( w == p_wzyx.w() );
}

// not absolutely insane unsigned short
struct naius
{
	unsigned short val;
};
bool operator==(naius a, naius b) { return a.val == b.val; }
bool operator!=(naius a, naius b) { return !(a == b); }
naius operator-(naius a)
{
	auto promoted = -unsigned(a.val);
	return naius{(unsigned short)promoted};
}
naius operator>>(naius a, int shift)
{
	auto promoted = unsigned(a.val) << shift;
	return naius{(unsigned short)promoted};
}
unsigned operator*(naius a, naius b)
{
	return unsigned(a.val) * unsigned(b.val);
}

void DefyPromotion()
{
	using puny = unsigned short; // bad
	// NOTE: can no longer demonstrate buny of size 1, since it promotes and decays to element type when shifted and initializer list contructor catches the bad implicit cast
	using buny = vector<puny,2>; // used to be goodish but now bad
	using vuny = vector<naius,1>; // good
	if constexpr (sizeof(puny) < sizeof(decltype(-puny{})))
	{
		// TODO: this test also relies on 2's complement representation and sign extension -_-
		puny p{1};
		assert( puny( -p >> 1 ) == puny(-p) ); // this does not look equivalent at all!
		assert( puny( -p >> 1 ) != -p >> 1 ); // and this does!

		// same for vector
		buny b{p,p};
		assert( buny( -b >> 1 ) == buny(-b) );
		// assert( buny( -b >> 1 ) != -b >> 1 ); // TODO: doesn't compile, since types are not same either... maybe should compile, since we're allowing promotion now

		// all fixed by a sane element type
		vuny v{naius{1}};
		assert( vuny( -v >> 1 ) != vuny(-v) );
		assert( vuny( -v >> 1 ) == -v >> 1 );

		// bonus multiplication, old goodish vector couldn't do this properly
		if constexpr (sizeof(unsigned short)*2 == sizeof(unsigned) && sizeof(int) == sizeof(unsigned))
		{
			auto biggy = std::numeric_limits<puny>::max();
			// assert( biggy * biggy < std::numeric_limits<int>::max() ); // instaUB

			naius nicey{biggy};
			assert( nicey * nicey > std::numeric_limits<int>::max() ); // all fine and dandy

			vuny vicey{nicey};
			assert(( vicey * vicey > vector<unsigned, 1>{{std::numeric_limits<int>::max()}} )); // vector respec sanity

			[[maybe_unused]] buny buggy{biggy, biggy};
			// assert(( buggy * buggy < vector(std::numeric_limits<int>::max(),std::numeric_limits<int>::max()) )); // as well as insanity -_-
		}
		else
			std::puts("what a world we live in~");
	}
	else
		std::puts("Type not puny enough, to test promotion defiance!");
};

constexpr bool Constexprness() // TODO: needs better coverage
{
	constexpr int4 p = int4(1,2,3,4);
	constexpr std::array<int, p[2]> arr = {1,2,3};
	constexpr std::array<int, int4::zero()[0]> arr2 = {};
	constexpr std::array<int, int4::one()[1]> arr3 = {13};
	static_assert(arr[p[1]] == 3, "Some other test should have caught this error.");
	static_assert(arr2.size() == 0, "Some other test should have caught this error.");
	static_assert(arr3[0] == 13, "Some other test should have caught this error.");
	assert(arr[2] == 3); // redundant check;

	return true;
}

void ValueCategoryOfGet()
{
	using std::get;

	vector a(1,2,3);
	static_assert(std::is_same_v<decltype(( get<0>(a) )), int&>);
	static_assert(std::is_same_v<decltype(( a[0] )), int&>);
	static_assert(std::is_same_v<decltype(( a.x() )), int&>);

	static_assert(std::is_same_v<decltype(( get<0>(vector(1,2,3)) )), int&&>);
	static_assert(std::is_same_v<decltype(( vector(1,2,3)[0] )), int&&>);
	// static_assert(std::is_same_v<decltype(( vector(1,2,3).x() )), int&&>); // TODO

	vector b(vector(1,2),vector(3,4));
	static_assert(std::is_same_v<decltype(( get<0>(b) )), vector<int,2>&>);
	static_assert(std::is_same_v<decltype(( get<1>(get<0>(b)) )), int&>);
	static_assert(std::is_same_v<decltype(( b[0] )), vector<int,2>&>);
	static_assert(std::is_same_v<decltype(( b[0][0] )), int&>);

	static_assert(std::is_same_v<decltype(( get<0>(
		vector(vector(1,2),vector(3,4))
	) )), vector<int,2>&&>);
	static_assert(std::is_same_v<decltype(( get<1>(get<0>(
		vector(vector(1,2),vector(3,4))
	)) )), int&&>);
	static_assert(std::is_same_v<
		decltype(( vector(vector(1,2),vector(3,4))[0] )),
		vector<int,2>&&>);
	static_assert(std::is_same_v<
		decltype(( vector(vector(1,2),vector(3,4))[0][1] )),
		int&&>);
}

void EmbracePromotion()
{
	using short3 = vector<short, 3>;
	short3 a {{1,2,3}}, b {{4,5,6}};

	auto c = a + b;
	assert((c == vector(5,7,9)));
	static_assert( std::is_same_v<decltype(c), vector<int,3>> );
	auto d = short3(a + b);
	assert((d == short3{{5,7,9}}));
	static_assert( std::is_same_v<decltype(d), short3> );
	auto e = a + a + b;
	assert((e == vector(6,9,12)));
	static_assert( std::is_same_v<decltype(e), vector<int,3>> );
	d += a;
	assert((d == short3{{6,9,12}}));
	e += d;
	assert((e == vector(12,18,24)));

	auto cc = a + 1;
	static_assert( std::is_same_v<decltype(cc), vector<int,3>> );
}

template <typename E>
class expr
{
	public:
	operator int() const
	{
		return static_cast<const E&>(*this);
	}
};

class expr_int : public expr<expr_int>
{
	int value;

	public:
    operator int() const { return value; }

    expr_int(int value = int{}) : value(value) {}

    template <typename E>
    expr_int(expr<E> const& expr) : value{expr} {}
};

template <typename E1, typename E2>
class expr_sum : public expr<expr_sum<E1, E2> > {
    E1 const* left;
    E2 const* right;

public:
    expr_sum() : left(), right() {}
    expr_sum(E1 const& left, E2 const& right) : left(&left), right(&right) {}

    operator int() const { return static_cast<int>(*left) + static_cast<int>(*right); }
};

template <typename E1, typename E2>
expr_sum<E1, E2>
operator+(expr<E1> const& left, expr<E2> const& right) {
   return expr_sum<E1, E2>(*static_cast<const E1*>(&left), *static_cast<const E2*>(&right));
}

void ExpressionTemplates()
{
	using expr_vec = vector<expr_int, 2>;
	expr_vec x{1,2};
	expr_vec y{3,4};
	vector<int,2> z (x + y + x);
	assert(z == vector(5,8));
}

// TODO: all the other ops -_-
void RowColumnVectorAndMatrix()
{

	const vector row(0.1f, 0.2f, 0.3f);

	auto matrix = vector {
		vector(1.0f, 2.0f, 3.0f),
		vector(4.0f, 5.0f, 6.0f),
		vector(7.0f, 8.0f, 9.0f),
	};

	assert(( matrix + row ==
		vector{
			vector(1.1f, 2.2f, 3.3f),
			vector(4.1f, 5.2f, 6.3f),
			vector(7.1f, 8.2f, 9.3f),
		}
	));
	assert(( row + matrix ==
		vector{
			vector(1.1f, 2.2f, 3.3f),
			vector(4.1f, 5.2f, 6.3f),
			vector(7.1f, 8.2f, 9.3f),
		}
	));
	matrix += row;
	assert(( matrix ==
		vector{
			vector(1.1f, 2.2f, 3.3f),
			vector(4.1f, 5.2f, 6.3f),
			vector(7.1f, 8.2f, 9.3f),
		}
	));

	const vector column{
		vector(0.1f),
		vector(0.2f),
		vector(0.3f),
	};

	matrix = vector {
		vector(1.0f, 2.0f, 3.0f),
		vector(4.0f, 5.0f, 6.0f),
		vector(7.0f, 8.0f, 9.0f),
	};

	assert(( matrix + column ==
		vector{
			vector(1.1f, 2.1f, 3.1f),
			vector(4.2f, 5.2f, 6.2f),
			vector(7.3f, 8.3f, 9.3f),
		}
	));

	assert(( column + matrix ==
		vector{
			vector(1.1f, 2.1f, 3.1f),
			vector(4.2f, 5.2f, 6.2f),
			vector(7.3f, 8.3f, 9.3f),
		}
	));

	matrix += column;
	assert(( matrix ==
		vector{
			vector(1.1f, 2.1f, 3.1f),
			vector(4.2f, 5.2f, 6.2f),
			vector(7.3f, 8.3f, 9.3f),
		}
	));

	assert
	(
		vector
		(
			vector(10),
			vector(20),
			vector(30)
		)
		+
		vector(1,2,3)
		==
		vector
		(
			vector(11, 12, 13),
			vector(21, 22, 23),
			vector(31, 32, 33)
		)
	);

	assert
	(
		vector(1,2,3)
		+
		vector
		(
			vector(10),
			vector(20),
			vector(30)
		)
		==
		vector
		(
			vector(11, 12, 13),
			vector(21, 22, 23),
			vector(31, 32, 33)
		)
	);

}

void BoolBitwiseCorrections()
{
	// insanity check
	static_assert(std::is_same_v<int, decltype(true & true)>);
	static_assert(std::is_same_v<int, decltype(true | true)>);
	static_assert(std::is_same_v<int, decltype(true ^ true)>);
	// static_assert(std::is_same_v<int, decltype(~true)>); // this is kind of sane, but compiler warns so cleary shouldn't be used

	// much saner vector -v-
	static_assert(std::is_same_v<vector<bool,1>, decltype(vector(true) & vector(true))>);
	static_assert(std::is_same_v<vector<bool,1>, decltype(vector(true) | vector(true))>);
	static_assert(std::is_same_v<vector<bool,1>, decltype(vector(true) ^ vector(true))>);
	static_assert(std::is_same_v<vector<bool,1>, decltype(~vector(true))>); // ok with vector, it's same as !
	// note: in conditional expressions ~ is elementwise negation, while ! causes a reduction to plain bool (see bool_algebra.cpp)

	// make sure we didn't break other things

	// bit operations involving other types still promotes
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) & vector(1))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(1) & vector(1))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) | vector(1))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(1) | vector(true))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) ^ vector(1))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(1) ^ vector(true))>);

	// arithmetic still promotes
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) + vector(true))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) - vector(true))>);
	// TODO: not sure about these... multiplication is basically conjunction so maybe should return bool,
	// division makes no sense, can only divide by 1, / is a no-op, % is constant 0, both still stay within bounds of bool though
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) * vector(true))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) / vector(true))>);
	static_assert(std::is_same_v<vector<int,1>, decltype(vector(true) % vector(true))>);

}

void BoolReductionImplicitConversion()
{
	vector x (1,2,3);
	assert( x + (x == vector(1,1,1)) == vector(2,2,3) );
	assert( (x == vector(1,1,1)) + x == vector(2,2,3) );
	x += x == vector(1,1,1);
	assert( x == vector(2,2,3) );
	x = x == vector(2,2,2);
	assert( x == vector(1,1,0) );
}

void OneDimentionalVectorDecays()
{
	using puny = short;
	assert (vector(puny(1)) + puny(1) == 2);
	static_assert( std::is_same_v<decltype(vector(puny(1)) + puny(1)), int> );
}

int main()
{
	ZeroConstruction();
	EqualityComparison();
	OtherConstruction();
	Mutation();
	Mixing();
	MultidimensionalElementAccess();
	RangeBasedLooping();
	Arithmetic();
	DiscreteArithmetic();
	ComparisonOperators();
	DisabiguateOnConstructorParameterCount();
	NumericLimits();
	CoordinateOrder();
	DefyPromotion();
	static_assert(Constexprness());
	ValueCategoryOfGet();
	EmbracePromotion();
	ExpressionTemplates();
	RowColumnVectorAndMatrix();
	BoolBitwiseCorrections();
	BoolReductionImplicitConversion();
	OneDimentionalVectorDecays();
	return 0;
}

