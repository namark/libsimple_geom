#include <fstream>
#include <cassert>
#include  "simple/geom/vector.hpp"

using namespace simple;
using geom::vector;

using int2 = vector<int,2>;
using int3 = vector<int,3>;
using int2x2 = vector<int2, 2>;
using int3x3 = vector<int3, 3>;

template <typename Vector>
std::istream& operator>>(std::istream& is, Vector& v)
{
	for(auto&& c : v)
		is >> c;
	return is;
}

void SquareMatrixMultiplication()
{
	std::vector<int3x3> matrices;
	std::ifstream test_data("matrix.data");
	while(test_data)
	{
		int3x3 matrix;
		test_data >> matrix;
		matrices.push_back(matrix);
	}
	assert(matrices.size() > 3);
	matrices.pop_back();

	assert(matrices.size() % 3 == 0);
	for(auto i = matrices.begin(); i != matrices.end(); i+=3)
	{
		int3x3 A = *i;
		int3x3 B = *(i+1);
		int3x3 AxB = *(i+2);
		assert(( B(A) == AxB ));
	}
}

void MatrixVectorMultiplication()
{
	struct test_case
	{
		int3x3 matrix;
		int3 in, out;
	};
	std::vector<test_case> tests;
	std::ifstream test_data("matrix_vector.data");
	while(test_data)
	{
		test_case test;
		test_data >> test.in;
		test_data >> test.matrix;
		test_data >> test.out;
		tests.push_back(test);
	}
	assert(tests.size() > 2);
	tests.pop_back();

	for(auto&& test : tests)
		assert( test.out == test.matrix(test.in) );
}

void DotProduct()
{
	struct test_case
	{
		int3 in1, in2;
		int out;
	};
	std::vector<test_case> tests;
	std::ifstream test_data("dot_product.data");
	while(test_data)
	{
		test_case test;
		test_data >> test.in1;
		test_data >> test.in2;
		test_data >> test.out;
		tests.push_back(test);
	}
	assert(tests.size() > 2);
	tests.pop_back();

	for(auto&& test : tests)
	{
		assert( test.out == test.in1(test.in2) );
		assert( test.out == test.in2(test.in1) );
	}
}

constexpr bool Constexprness()
{
	constexpr int3x3 A{}, B{};
	constexpr int3 a{}, b{};
	A(B); B(A); A(a); B(a); a(b);
	return true;
}

int main()
{

	SquareMatrixMultiplication();
	MatrixVectorMultiplication();
	DotProduct();
	static_assert(Constexprness());
	return 0;
}

