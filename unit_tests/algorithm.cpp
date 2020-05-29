#include <cmath>
#include <cassert>
#include <vector>

#include "simple/support/math.hpp"
#include "simple/geom/algorithm.hpp"

using namespace simple;
using geom::vector;
using float4 = vector<float, 4>;
using int4 = vector<int, 4>;

void Basic()
{

	int4 p (1, 2, 3, 4);
	int4 p2 (4, 3, 2, 1);
	assert( int4(1, 2, 2, 1) == geom::min(p, p2) );
	assert( int4(4, 3, 3, 4) == geom::max(p, p2) );

	int4 p3;
	(p3 = p).min(p2);
	assert( int4(1, 2, 2, 1) == p3 );
	(p3 = p).max(p2);
	assert( int4(4, 3, 3, 4) == p3 );

	p = -p;
	assert( int4(-1, 2, 2, -3) == geom::clamp(int4(-10, 2, 3, -3), p, p2) );
	assert( int4(0, -2, -1, 1) == geom::clamp(int4(0, -3, -1, 10), p, p2) );
	assert( int4(3, -1, -2, 0) == geom::clamp(int4(3, -1, -2, 0), p, p2) );
	assert( int4(-1, 3, -3, 1) == geom::clamp(int4(-3, 7, -5, 3), p, p2) );

	(p3 = int4(-10, 2, 3, -3)).clamp(p,p2);
	assert( int4(-1, 2, 2, -3) == p3 );
	(p3 = int4(0, -3, -1, 10)).clamp(p,p2);
	assert( int4(0, -2, -1, 1) == p3);
	(p3 = int4(3, -1, -2, 0)).clamp(p,p2);
	assert( int4(3, -1, -2, 0) == p3);
	(p3 = int4(-3, 7, -5, 3)).clamp(p,p2);
	assert( int4(-1, 3, -3, 1) == p3);

	float4 fp (1.1f, 3.4f, 4.5f, 8.9f);
	assert( float4(1.f, 3.f, 4.f, 8.f) == trunc(fp) );
	assert( float4(1.f, 3.f, 4.f, 8.f) == floor(fp) );
	assert( float4(2.f, 4.f, 5.f, 9.f) == ceil(fp) );
	assert( float4(1.f, 3.f, 5.f, 9.f) == round(fp) );

	float4 fp2;
	(fp2 = fp).trunc();
	assert( float4(1.f, 3.f, 4.f, 8.f) == fp2 );
	(fp2 = fp).floor();
	assert( float4(1.f, 3.f, 4.f, 8.f) == fp2 );
	(fp2 = fp).ceil();
	assert( float4(2.f, 4.f, 5.f, 9.f) == fp2 );
	(fp2 = fp).round();
	assert( float4(1.f, 3.f, 5.f, 9.f) == fp2 );

	fp = float4(1.1f, -3.4f, 4.5f, -8.9f);
	assert( float4(1.f, -3.f, 4.f, -8.f) == trunc(fp) );
	assert( float4(1.f, -4.f, 4.f, -9.f) == floor(fp) );
	assert( float4(2.f, -3.f, 5.f, -8.f) == ceil(fp) );
	assert( float4(1.f, -3.f, 5.f, -9.f) == round(fp) );

	(fp2 = fp).trunc();
	assert( float4(1.f, -3.f, 4.f, -8.f) == fp2 );
	(fp2 = fp).floor();
	assert( float4(1.f, -4.f, 4.f, -9.f) == fp2 );
	(fp2 = fp).ceil();
	assert( float4(2.f, -3.f, 5.f, -8.f) == fp2 );
	(fp2 = fp).round();
	assert( float4(1.f, -3.f, 5.f, -9.f) == fp2 );

	int4 p4 (2, 6, 3, 0);
	assert( p4.magnitude() ==  49);
	assert( p4.quadrance() ==  49);
	assert( p4.length() == 7 );
	assert( magnitude(p4) ==  49);
	assert( quadrance(p4) ==  49);
	assert( length(p4) == 7 );


	assert( signum(int4(-12, 34, 0, 1)) == int4(-1, 1, 0, 1) );
	assert( signum(int4(9, -13, -77, 0)) == int4(1, -1, -1, 0) );
	assert( signum(float4(-.3f, .12f, .0f, +.0f)) == float4(-1.f, 1.f, 0.f, 0.f) );

	(p3 = int4(-12, 34, 0, 1)).signum();
	assert( int4(-1, 1, 0, 1) == p3 );
	(p3 = int4(9, -13, -77, 0)).signum();
	assert( int4(1, -1, -1, 0) == p3 );
	(fp2 = float4(-.3f, .12f, .0f, +.0f)).signum();
	assert( float4(-1.f, 1.f, 0.f, 0.f) == fp2 );
}

void MultidimentionalIteration()
{
	using int3 = vector<int, 3>;
	const auto lower = int3{13,3,-20};
	const auto upper = int3{45,32,12};

	std::vector<int3> test_data;
	test_data.reserve(10000);
	std::vector<int3> data;
	data.reserve(10000);


	for(int k = lower[2]; k < upper[2]; ++k)
		for(int j = lower[1]; j < upper[1]; ++j)
			for(int i = lower[0]; i < upper[0]; ++i)
				test_data.push_back({i,j,k});

	loop(lower, upper, int3::one(), [&data](auto& i)
	{
		data.push_back(i);
	});

	assert(data == test_data);

	test_data.clear();
	data.clear();

	auto step = int3{1,2,3};

	for(int k = lower[2]; k < upper[2]; k += step[2])
		for(int j = lower[1]; j < upper[1]; j += step[1])
			for(int i = lower[0]; i < upper[0]; i += step[0])
				test_data.push_back({i,j,k});

	loop(lower, upper, step, [&data](auto& i)
	{
		data.push_back(i);
	});

	assert(data == test_data);
}

struct rational
{
	long long num = 0;
	long long den = 1;

	constexpr
	bool operator < (const rational& other) const
	{ return num * other.den < other.num * den; }

	constexpr
	bool operator == (const rational& other) const
	{ return num * other.den == other.num * den; }

	constexpr
	rational operator+() { return *this; } const
	constexpr

	rational operator*(const rational& other) const
	{ return {num*other.num, den*other.den}; }
	constexpr

	void normalize()
	{
		auto gcd = std::gcd(num, den);
		num /= gcd;
		den /= gcd;
	}

	constexpr
	rational& operator/=(const rational& other)
	{
		den *= other.num;
		num *= other.den;
		normalize();
		return *this;
	}

	constexpr
	rational& operator-=(const rational& other)
	{
		num *= other.den;
		num -= other.num * den;
		den *= other.den;
		normalize();
		return *this;
	}

	constexpr
	rational& operator+=(const rational& other)
	{
		num *= other.den;
		num += other.num * den;
		den *= other.den;
		normalize();
		return *this;
	}

	constexpr
	rational operator+(const rational& other) const
	{
		return rational(*this)+=other;;
	}
};

constexpr rational abs(rational r)
{
	return {support::abs(r.num) , support::abs(r.den)};
}

void MatrixElimination()
{
	using r = rational;

	auto inverse = gauss_jordan_elimination(vector(
			vector(r{2},r{0},r{3}, r{1},r{0},r{0}),
			vector(r{4},r{5},r{6}, r{0},r{1},r{0}),
			vector(r{7},r{8},r{9}, r{0},r{0},r{1})
	)).mutant_clone(&vector<r,6>::last<3>);
	assert ( vector(r{2},r{0},r{3})(inverse) ==
		vector(r{17,5}, r{-4,5}, r{-8,5}) );

	assert( gauss_jordan_elimination(
		vector(
			vector(r{2},r{1},r{0}, r{1},r{0},r{0}),
			vector(r{0},r{2},r{0}, r{0},r{1},r{0}),
			vector(r{2},r{0},r{1}, r{0},r{0},r{1})
		))
		==
		vector(
			vector(r{1},r{0},r{0}, r{1,2},r{-1,4},r{0}),
			vector(r{0},r{1},r{0}, r{0},  r{1,2}, r{0}),
			vector(r{0},r{0},r{1}, r{-1}, r{1,2},   r{1})
		)
	);

	assert( gauss_jordan_elimination(
		vector(
			vector(r{1},r{1},r{0}, r{1},r{0},r{0}),
			vector(r{0},r{1},r{0}, r{0},r{1},r{0}),
			vector(r{2},r{1},r{1}, r{0},r{0},r{1})
		))
		==
		vector(
			vector(r{1},r{0},r{0}, r{1}, r{-1},r{0}),
			vector(r{0},r{1},r{0}, r{0}, r{1}, r{0}),
			vector(r{0},r{0},r{1}, r{-2},r{1}, r{1})
		)
	);

	assert( gauss_jordan_elimination(
		vector(
			vector(r{2},r{1},r{0}, r{1},r{0},r{0}),
			vector(r{2},r{0},r{0}, r{0},r{1},r{0}),
			vector(r{2},r{0},r{1}, r{0},r{0},r{1})
		))
		==
		vector(
			vector(r{1},r{0},r{0}, r{0},r{1,2},r{0}),
			vector(r{0},r{1},r{0}, r{1},r{-1}, r{0}),
			vector(r{0},r{0},r{1}, r{0},r{-1}, r{1})
		)
	);

}

int main()
{
	Basic();
	MultidimentionalIteration();
	MatrixElimination();
}
