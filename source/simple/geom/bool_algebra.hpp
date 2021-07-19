#ifndef SIMPLE_GEOM_BOOL_ALGEBRA_HPP
#define SIMPLE_GEOM_BOOL_ALGEBRA_HPP

#include "reduction.hpp"
#include "vector.hpp"
#include "algorithm.hpp"

namespace simple::geom
{

	struct conjunct
	{
		static constexpr bool identity = true;
		// NOTE: passing by value here prevented short circuiting
		// gcc 9.3.0
		// gcc 7.5.0
		constexpr bool operator()(const bool& a, const bool& b) const noexcept
		{
			return a && b;
		}
	};

	struct disjunct
	{
		static constexpr bool identity = false;
		// NOTE: passing by value here prevented short circuiting
		// gcc 9.3.0
		// gcc 7.5.0
		constexpr bool operator()(const bool& a, const bool& b) const noexcept
		{
			return a || b;
		}
	};

	// TODO: guess need to support compatibility_tag from support/array_operators.hpp :/
#define SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(op, reduce_method) \
	template <typename C1, typename C2, size_t D, typename O, \
		typename Bool = \
			typename vector<C1, D, O>::template map_coordinate_t<bool> \
	> \
	[[nodiscard]] \
	constexpr reduction<reduce_method, Bool, bool> \
	operator op(const vector<C1,D,O>& one, const vector<C2,D,O>& other) \
	{ \
		Bool ret{}; \
		for(size_t i = 0; i < D; ++i) \
			ret[i] = (one[i] op other[i]); \
		return reduction<reduce_method, Bool, bool>(ret); \
	}

SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(==, conjunct)
SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(!=, disjunct)
SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(>,  conjunct)
SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(>=, conjunct)
SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(<,  conjunct)
SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR(<=, conjunct)
#undef SIMPLE_GEOM_VECTOR_DEFINE_COMPARISON_OPERATOR

	template <typename C, size_t D, typename O>
	using conjunction = reduction<
		conjunct, vector<C,D,O> >;

	template <typename C, size_t D, typename O>
	using disjunction = reduction<
		disjunct, vector<C,D,O> >;

	template <typename C, size_t D, typename O>
	conjunction<C,D,O> to_conjunction(vector<C,D,O> v)
	{
		return conjunction<C,D,O>(v);
	}

	template <typename C, size_t D, typename O>
	disjunction<C,D,O> to_disjunction(vector<C,D,O> v)
	{
		return disjunction<C,D,O>(v);
	}

	// De Morgan's laws
	template <typename C, size_t D, typename O>
	[[nodiscard]] constexpr conjunction<C,D,O>
	operator~(const disjunction<C,D,O>& one) noexcept
	{
		return conjunction<C,D,O>(~(one.range));
	}
	template <typename C, size_t D, typename O>
	[[nodiscard]] constexpr disjunction<C,D,O>
	operator~(const conjunction<C,D,O>& one) noexcept
	{
		return disjunction<C,D,O>(~(one.range));
	}

	// TODO; only these super weird expanding ops below need algorithm.hpp, so better separate them into an optional header
	// will probably need to disable the element wise * and + for this (or bool vector in general) as well to not make inclusion of a header silently change behavior

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	[[nodiscard]]
	constexpr auto operator*
	(
		disjunction<C,D,O> one,     // A1xA2x...An
		disjunction<C2,D2,O2> other // B1xB2x...Bm
	)
	noexcept
	{
		// A1xA2x...Anx1
		const auto row = vector(one.range);

		// 1xB1xB2x...Bm
		const auto column = deep_mutant_clone(other.range,
			[](auto x) { return vector(x); });

		// A1xA2x...AnxB1xB2...xBm
		return to_disjunction(row(column));
	}

	class conjunctive_bool
	{
		bool raw;
		public:

		constexpr conjunctive_bool() : raw(conjunct::identity) {}
		constexpr explicit conjunctive_bool(bool raw) : raw(raw) {}
		constexpr explicit operator bool() const noexcept { return raw; }
		constexpr conjunctive_bool& operator+=(conjunctive_bool other) noexcept
		{
			raw = conjunct{}(raw, other.raw);
			return *this;
		}
		constexpr conjunctive_bool operator*(conjunctive_bool other) const noexcept
		{
			other.raw = disjunct{}(other.raw, raw);
			return other;
		}
	};

	// same as disjunction +, but additionally converting to conjunction_bool and back to customize identity and ops used by vector application (matrix multiplication)
	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	[[nodiscard]]
	constexpr auto operator+
	(
		conjunction<C,D,O> one,
		conjunction<C2,D2,O2> other
	)
	noexcept
	{
		const auto row = vector(deep_mutant_clone(one.range,
			[](auto x) { return conjunctive_bool(x); }));
		const auto column = deep_mutant_clone(other.range,
			[](auto x) { return vector(conjunctive_bool(x)); });
		return to_conjunction(deep_mutant_clone(row(column),
			[](auto x) { return bool(x); }));
	}

	// TODO: figure these out
	// ideally want elementwise when possible
	// but there are lot of strange cases

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	constexpr auto operator+
	(
		disjunction<C,D,O>,
		disjunction<C2,D2,O2>
	)
	= delete;

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	constexpr auto operator+
	(
		disjunction<C,D,O>,
		conjunction<C2,D2,O2>
	)
	= delete;

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	constexpr auto operator+
	(
		conjunction<C,D,O>,
		disjunction<C2,D2,O2>
	)
	= delete;

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	constexpr auto operator*
	(
		conjunction<C,D,O>,
		conjunction<C2,D2,O2>
	)
	= delete;

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	constexpr auto operator*
	(
		conjunction<C,D,O> one,
		disjunction<C2,D2,O2>
	)
	= delete;

	template <
		typename C, typename C2,
		size_t D, size_t D2,
		typename O, typename O2
	>
	constexpr auto operator*
	(
		disjunction<C,D,O> one,
		conjunction<C2,D2,O2>
	)
	= delete;

} // namespace simple::geom

namespace simple
{
	template <typename C, size_t D, typename O>
	struct support::array_operator_implicit_conversion<
		geom::disjunction<C,D,O>>
	{
		using type = typename geom::disjunction<C,D,O>::
			range_type::template map_coordinate_t<bool>;
	};

	template <typename C, size_t D, typename O>
	struct support::array_operator_implicit_conversion<
		geom::conjunction<C,D,O>>
	{
		using type = typename geom::conjunction<C,D,O>::
			range_type::template map_coordinate_t<bool>;
	};
} // namespace simple

#endif /* end of include guard */
