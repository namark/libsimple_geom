#include "simple/geom/vector.hpp"
#include <cmath>
#include <cassert>

using namespace simple;
using float4 = geom::vector<float, 4>;
using int4 = geom::vector<int, 4>;


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
	assert( p2 == p.mutantClone<int4>( [](auto x) -> int { return std::round(x); } ));
	assert( p3 == p.mutantClone<int4>( [](auto x) -> int { return x; } ));
	assert( p2 == p.mutant_clone( [](auto x) -> int { return std::round(x); } ));
	assert( p3 == p.mutant_clone( [](auto x) -> int { return x; } ));
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
}

void Algorithms()
{

	int4 p (1, 2, 3, 4);
	int4 p2 (4, 3, 2, 1);
	assert( int4(1, 2, 2, 1) == geom::min(p, p2) );
	assert( int4(4, 3, 3, 4) == geom::max(p, p2) );

	p = -p;
	assert( int4(-1, 2, 2, -3) == geom::clamp(int4(-10, 2, 3, -3), p, p2) );
	assert( int4(0, -2, -1, 1) == geom::clamp(int4(0, -3, -1, 10), p, p2) );
	assert( int4(3, -1, -2, 0) == geom::clamp(int4(3, -1, -2, 0), p, p2) );
	assert( int4(-1, 3, -3, 1) == geom::clamp(int4(-3, 7, -5, 3), p, p2) );

	float4 fp (1.1f, 3.4f, 4.5f, 8.9f);
	assert( float4(1.f, 3.f, 4.f, 8.f) == floor(fp) );
	assert( float4(2.f, 4.f, 5.f, 9.f) == ceil(fp) );
	assert( float4(1.f, 3.f, 5.f, 9.f) == round(fp) );

	int4 p3 (2, 6, 3, 0);
	assert( p3.magnitude() ==  49);
	assert( p3.length() == 7 );

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
	static_assert(limits::max() == int4::one()*coordLimits::max());
	static_assert(limits::min() == int4::one()*coordLimits::min());
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
}

constexpr void Constexprness() // TODO: needs better coverage
{
	constexpr int4 p = int4(1,2,3,4);
	constexpr std::array<int, p[2]> arr = {1,2,3};
	constexpr std::array<int, int4::zero()[0]> arr2 = {};
	constexpr std::array<int, int4::one()[1]> arr3 = {13};
	static_assert(arr[p[1]] == 3, "Some other test should have caught this error.");
	static_assert(arr2.size() == 0, "Some other test should have caught this error.");
	static_assert(arr3[0] == 13, "Some other test should have caught this error.");
	assert(arr[2] == 3); // redundant check;
}


int main()
{
	ZeroConstruction();
	EqualityComparison();
	OtherConstruction();
	Mutation();
	Mixing();
	RangeBasedLooping();
	Arithmetic();
	DiscreteArithmetic();
	ComparisonOperators();
	Algorithms();
	DisabiguateOnConstructorParameterCount();
	NumericLimits();
	CoordinateOrder();
	Constexprness();
	return 0;
}

