#include <iostream>
#include <string>
#include <cassert>
#include <sstream>
#include <algorithm>
#include "simple/geom/vector.hpp"

using namespace std::literals;
using namespace simple::geom;

std::stringstream ss;

struct sym
{
	std::string val;

#define DEFINE_SYM_OPERATOR(op) \
	sym operator op(sym s) const \
	{ \
		if(!val.empty()) \
			s.val = val + #op + s.val; \
		return s; \
	} \
	sym& operator op##=(sym s) \
	{ \
		(*this) = (*this) op s; \
		return (*this); \
	}
	DEFINE_SYM_OPERATOR(+)
	DEFINE_SYM_OPERATOR(-)
	DEFINE_SYM_OPERATOR(*)
	DEFINE_SYM_OPERATOR(/)
	DEFINE_SYM_OPERATOR(%)
#undef DEFINE_SYM_OPERATOR

	friend std::ostream& operator<<(std::ostream& os, const sym& s)
	{
		return os << s.val;
	}
};

template <typename... Cs>
auto sym_vector(Cs... cs)
{
	return vector(sym{cs}...);
}

const auto wrap_in_vector = [](const auto &x) {return vector(x);};

template <typename T, size_t D, typename O>
constexpr vector<vector<T,D,O>, D,O> dot_wedge_matrix(const vector<T,D,O>& one, const vector<T,D,O>& other)
{
	const auto row = vector(one); // Dx1
	const auto column = other.mutant_clone(wrap_in_vector); // 1xD
	return row(column); // Dx1 * 1xD = DxD
}

template <typename T, size_t D, typename O, typename Ret = vector<T, D*D/2 - D/2 + 1>>
constexpr Ret dot_wedge(const vector<T,D,O>& one, const vector<T,D,O>& other)
{
	const auto matrix = dot_wedge_matrix(one, other);

	Ret r{};

	// accumulate diagonal, which is the dot product
	for(size_t i = 0; i < D; ++i)
		r[Ret::dimensions - 1] += matrix[i][i];

	// upper triangle minus lower triangle gives wedge product (kind of)
	// some signs are flipped, but that's just a choice of basis right?
	// doesn't matter right? ... right?
	for(size_t i = 0, j = 0, k = 1; j < D-1; ++i)
	{
		r[i] = matrix[j][k] - matrix[k][j];

		++k;
		if(k == D)
		{
			++j;
			k = j+1;
		}
	}

	return r;
}

template<typename T, size_t D, typename O>
const auto& vector_only_geometric_product = dot_wedge<T, D, O>;

template<typename T, size_t D, typename O>
const auto& make_rotor = dot_wedge<T, D, O>;

void Explode()
{
	auto a = vector( sym_vector("x1", "x2", "x3") ); // 3x1
	auto b = sym_vector("y1", "y2", "y3").mutant_clone(wrap_in_vector); // 1x3

	ss << a(b); // 3x1 * 1x3 = 3x3

	assert( ss.str() ==
R"(^^^
(x1*y1, x2*y1, x3*y1)
(x1*y2, x2*y2, x3*y2)
(x1*y3, x2*y3, x3*y3)
vvv
)"
	); ss.str("");

	ss << b(a); // 1x3 * 3x1 = 1x1
	assert( ss.str() ==
R"(^
(y1*x1+y2*x2+y3*x3)
v
)"
	); ss.str("");


	auto ab = vector(a(b)); // 3x3x1

	ss << ab(b); // 3x3x1 * 1x3 = 3x3x3
	assert( ss.str() ==
R"(^^^
^^^
(x1*y1*y1, x2*y1*y1, x3*y1*y1)
(x1*y2*y1, x2*y2*y1, x3*y2*y1)
(x1*y3*y1, x2*y3*y1, x3*y3*y1)
vvv

^^^
(x1*y1*y2, x2*y1*y2, x3*y1*y2)
(x1*y2*y2, x2*y2*y2, x3*y2*y2)
(x1*y3*y2, x2*y3*y2, x3*y3*y2)
vvv

^^^
(x1*y1*y3, x2*y1*y3, x3*y1*y3)
(x1*y2*y3, x2*y2*y3, x3*y2*y3)
(x1*y3*y3, x2*y3*y3, x3*y3*y3)
vvv

vvv
)"
	); ss.str("");

	ss << b(ab); // 1x3 * 3x3x1 = 1x3x1
	assert( ss.str() ==
R"(^^^
^
(y1*x1*y1+y2*x2*y1+y3*x3*y1)
(y1*x1*y2+y2*x2*y2+y3*x3*y2)
(y1*x1*y3+y2*x2*y3+y3*x3*y3)
v

vvv
)"
	); ss.str("");

	ss << (ab(b))(ab); // brace yourselves: 3x3x3 * 3x3x1 = 3x3x3x1
	assert( ss.str() ==
R"(^^^
^^^
^^^
(x1*y1*y1*x1*y1+x1*y1*y2*x2*y1+x1*y1*y3*x3*y1, x2*y1*y1*x1*y1+x2*y1*y2*x2*y1+x2*y1*y3*x3*y1, x3*y1*y1*x1*y1+x3*y1*y2*x2*y1+x3*y1*y3*x3*y1)
(x1*y2*y1*x1*y1+x1*y2*y2*x2*y1+x1*y2*y3*x3*y1, x2*y2*y1*x1*y1+x2*y2*y2*x2*y1+x2*y2*y3*x3*y1, x3*y2*y1*x1*y1+x3*y2*y2*x2*y1+x3*y2*y3*x3*y1)
(x1*y3*y1*x1*y1+x1*y3*y2*x2*y1+x1*y3*y3*x3*y1, x2*y3*y1*x1*y1+x2*y3*y2*x2*y1+x2*y3*y3*x3*y1, x3*y3*y1*x1*y1+x3*y3*y2*x2*y1+x3*y3*y3*x3*y1)
vvv

^^^
(x1*y1*y1*x1*y2+x1*y1*y2*x2*y2+x1*y1*y3*x3*y2, x2*y1*y1*x1*y2+x2*y1*y2*x2*y2+x2*y1*y3*x3*y2, x3*y1*y1*x1*y2+x3*y1*y2*x2*y2+x3*y1*y3*x3*y2)
(x1*y2*y1*x1*y2+x1*y2*y2*x2*y2+x1*y2*y3*x3*y2, x2*y2*y1*x1*y2+x2*y2*y2*x2*y2+x2*y2*y3*x3*y2, x3*y2*y1*x1*y2+x3*y2*y2*x2*y2+x3*y2*y3*x3*y2)
(x1*y3*y1*x1*y2+x1*y3*y2*x2*y2+x1*y3*y3*x3*y2, x2*y3*y1*x1*y2+x2*y3*y2*x2*y2+x2*y3*y3*x3*y2, x3*y3*y1*x1*y2+x3*y3*y2*x2*y2+x3*y3*y3*x3*y2)
vvv

^^^
(x1*y1*y1*x1*y3+x1*y1*y2*x2*y3+x1*y1*y3*x3*y3, x2*y1*y1*x1*y3+x2*y1*y2*x2*y3+x2*y1*y3*x3*y3, x3*y1*y1*x1*y3+x3*y1*y2*x2*y3+x3*y1*y3*x3*y3)
(x1*y2*y1*x1*y3+x1*y2*y2*x2*y3+x1*y2*y3*x3*y3, x2*y2*y1*x1*y3+x2*y2*y2*x2*y3+x2*y2*y3*x3*y3, x3*y2*y1*x1*y3+x3*y2*y2*x2*y3+x3*y2*y3*x3*y3)
(x1*y3*y1*x1*y3+x1*y3*y2*x2*y3+x1*y3*y3*x3*y3, x2*y3*y1*x1*y3+x2*y3*y2*x2*y3+x2*y3*y3*x3*y3, x3*y3*y1*x1*y3+x3*y3*y2*x2*y3+x3*y3*y3*x3*y3)
vvv

vvv

vvv
)"
	); ss.str("");


	auto a2 = vector( sym_vector("a", "b") ); // 2x1
	auto b2 = sym_vector("c", "d").mutant_clone(wrap_in_vector); // 1x2

	ss << a2(b2) << '\n';

	ss << dot_wedge(
		sym_vector("a", "b"),
		sym_vector("c", "d")
	) << '\n';

	ss << dot_wedge(
		sym_vector("u1", "u2", "u3"),
		sym_vector("v1", "v2", "v3")
	) << '\n';

	// (a + ib)(c + id) = ac + aid + ibc + iibd = (ac - bd) + (ad + bc)i
	// (a + ib)(c - id) = ac - aid + ibc - iibd = (ac + bd) + (bc - ad)i
	// ((ac - bd) + (ad + bc)i)(a-ib) = (ac - bd)(a-ib) + (ad+bc)i(a-ib) =
	// = aca - acib - bda + bdib + adia + bcia - adiib - bciib =
	// = aca - bda + bdib + adia + adb + bcb

	assert(ss.str() ==
R"(^^
(a*c, b*c)
(a*d, b*d)
vv

(b*c-a*d, a*c+b*d)
(u2*v1-u1*v2, u3*v1-u1*v3, u3*v2-u2*v3, u1*v1+u2*v2+u3*v3)
)"
	); ss.str("");


	auto aa = vector(
		sym_vector("x1", "x2"),
		sym_vector("x3", "x4")
	);
	auto bb = vector(
		sym_vector("y1", "y2"),
		sym_vector("y3", "y4")
	);
	auto A = vector( aa ); // 2x2x1
	auto B = bb.mutant_clone([](auto x){ return x.mutant_clone(wrap_in_vector);}); // 1x2x2
	ss << A(B); // 2x2x1 * 1x2x2 = 2x2x2x2
	assert(ss.str() ==
R"(^^
^^
^^
(x1*y1, x2*y1)
(x3*y1, x4*y1)
vv

^^
(x1*y2, x2*y2)
(x3*y2, x4*y2)
vv

vv

^^
^^
(x1*y3, x2*y3)
(x3*y3, x4*y3)
vv

^^
(x1*y4, x2*y4)
(x3*y4, x4*y4)
vv

vv

vv
)"
	); ss.str("");

}

int main()
{
	Explode();
	return 0;
}
