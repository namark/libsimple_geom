#include "simple/geom/vector.hpp"
#include <cmath>
#include <cassert>
using namespace simple;
using PointF4 = geom::vector<float, 4>;
using Point4 = geom::vector<int, 4>;


void EqualityComparison()
{
}

void ZeroConstruction()
{
	PointF4::array control{};
	PointF4 p{};
	PointF4 p2 = {};
	PointF4 p3 = PointF4();
	assert(control == static_cast<const PointF4::array&>(p));
	assert(control == static_cast<const PointF4::array&>(p2));
	assert(control == static_cast<const PointF4::array&>(p3));
	assert(control == static_cast<const PointF4::array&>(PointF4::zero()));

	// NOTE: can't really test for random garbage since random garbage can be anything, including all zeros
	bool anyGarbage = false;
	std::array<PointF4, 100> garbagePile;
	for (auto& garbage : garbagePile)
	{
		anyGarbage |= (control != static_cast<PointF4::array>(garbage));
		if(anyGarbage) break;
	}
	assert(anyGarbage);

}

void OtherConstruction()
{
	PointF4 p(1.0f, 2.0f, 3.0f, 4.0f);
	PointF4 p2{1.0f, 2.0f, 3.0f, 4.0f};
	PointF4 p3{{1.0f, 2.0f, 3.0f, 4.0f}};
	PointF4 p4{PointF4::array{1.0f, 2.0f, 3.0f, 4.0f}};
	PointF4 p5(PointF4::array{1.0f, 2.0f, 3.0f, 4.0f});
	PointF4 p6 = PointF4(Point4(1, 2, 3, 4));
	PointF4 p7 {{1, 2, 3, 4}};
	PointF4 p8 (p7);
	PointF4 p9 = {p8};
	assert(p == p2);
	assert(p2 == p3);
	assert(p3 == p4);
	assert(p4 == p5);
	assert(p5 == p6);
	assert(p6 == p7);
	assert(p7 == p8);
	assert(p8 == p9);
	assert(p9 != PointF4::zero());
	static_assert(Point4(1,1,1,1) == Point4::one(), "");
	static_assert(PointF4(1.0f,1.0f,1.0f,1.0f) == PointF4::one(), "");
	static_assert(Point4(1,0,0,0) == Point4::i(), "");
	static_assert(Point4(0,1,0,0) == Point4::j(), "");
	static_assert(Point4(0,0,1,0) == Point4::k(), "");
	static_assert(Point4(0,0,0,1) == Point4::l(), "");
}

void Accessors()
{
}

void Mutation()
{
	PointF4 p {1.3f, 1.5f, 2.1f, 4.6f};
	Point4 p2 {1, 2, 2, 5};
	Point4 p3 {1, 1, 2, 4};
	assert( p2 == p.mutantClone<Point4>( [](float x) { return std::round(x); } ));
	assert( p3 == p.mutantClone<Point4>( [](float x) { return x; } ));
}

void RangeBasedLooping()
{
	const PointF4 p{1.0f, 2.0f, 3.0f, 4.0f};
	PointF4 p2;

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
	PointF4 p{{1, 2, 3, 4}};
	PointF4 p2 = p;
	PointF4 sum{{2, 4, 6, 8}};
	PointF4 inc{{2, 3, 4, 5}};
	PointF4 dec{{0, 1, 2, 3}};
	assert(p + p2 == sum);
	assert(p + 1 == inc);
	assert(p - 1 == dec);
	assert(p - p2 == PointF4::zero());
	assert(-p == PointF4(Point4(-1, -2, -3, -4)));

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
	assert(p2 == PointF4::zero());

	p2 = p;
	p2 += 3;
	assert(p2 == p + 3);
	p2 -= 1.5f;
	assert(p2 == p + 1.5);

	assert( p + p == p * 2);
	assert( p * p == PointF4(1.0f, 4.0f, 9.0f, 16.0f));

	assert( (p + p)/2 == p);
	assert( p / p == PointF4(1.0f, 1.0f, 1.0f, 1.0f));

	p2 = p;
	p2 *= 2;
	assert(p2 == p*2);
	p2 *= p;
	assert(p2 == p*p*2);
	p2 /= p;
	assert(p2 == p*2);
	p2 /= 2;
	assert(p2 == p);
}

void DiscreteArithmetic()
{
	Point4 p(1, 2, 3, 4);
	Point4 p2(1, 0, 1, 0);
	Point4 p3(1, 2, 0, 1);
	assert(p%2 == p2);
	p %= 3;
	assert(p == p3);

	p = {1, 2, 3, 4};
	p3 = {4, 5, 7, 9};
	p2 = {3, 3, 4, 5};
	assert(p3 % p2 == p);
	p3 %= p2;
	assert(p3 == p);

	//<<
	//>>
	//<<=
	//>>=
	//&
	//&=
	//|
	//|=
	//^
	//^=
	//~
}

// edge cases


void Constexprness()
{
	constexpr Point4 p = Point4(1,2,3,4);
	constexpr std::array<int, p[2]> arr = {1,2,3};
	constexpr std::array<int, Point4::zero()[0]> arr2 = {};
	constexpr std::array<int, Point4::one()[1]> arr3 = {13};
	static_assert(arr[p[1]] == 3, "Some other test should have caught this error.");
	static_assert(arr2.size() == 0, "Some other test should have caught this error.");
	static_assert(arr3[0] == 13, "Some other test should have caught this error.");
	assert(arr[2] == 3); // redundant check;
}


int main()
{
	EqualityComparison();
	ZeroConstruction();
	OtherConstruction();
	Accessors();
	Mutation();
	RangeBasedLooping();
	Arithmetic();
	DiscreteArithmetic();
	Constexprness();
	return 0;
}

