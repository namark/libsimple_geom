#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include "simple/support/algorithm.hpp"

using namespace std;
using simple::support::make_range;

using strange = simple::support::range<string::iterator>;
ostream& operator<<(ostream& output, const strange & value)
{
	for(auto&& ch : value)
		output << ch;
	return output;
}

using char_ptr = istream_iterator<char>;
const char_ptr char_end = char_ptr();

const string keyword = "simple::geom::vector<";

class in_scope
{
	public:
		in_scope(char open, char close, char match)
			: open(open), close(close), match(match), counter(1) {}
		in_scope(char open, char close)
			: in_scope(open, close, close) {};
		bool operator()(char current)
		{
			if(counter == 0)
				return false;

			if(counter == 1 && current == match)
				return true;

			if(current == close)
			{
				--counter;
			}
			else if(current == open)
			{
				++counter;
			}

			return false;
		}
	private:
		const char open, close, match;
		int counter;
};

template<typename ForwardIterator, typename OutputIterator, typename Functor>
ForwardIterator copy_until(ForwardIterator first, ForwardIterator last, OutputIterator out, Functor end_condition)
{
	while(first != last)
	{
		const typename ForwardIterator::value_type& value = *first;
		if(end_condition(value))
			break;
		*out++ = value;
		++first;
	}
	return first;
}

void shorten_vector_stuff(strange stuff)
{
	auto comma = find_if(stuff.begin(), stuff.end(),
		in_scope('<', '>', ','));
	auto second_comma = find_if(comma+1, stuff.end(),
		in_scope('<', '>', ','));

	strange til_comma {stuff.begin(), comma};
	auto nested_vector = search(til_comma.begin(), til_comma.end(), keyword.begin(), keyword.end());

	cout << "<";
	if(nested_vector != til_comma.end())
	{
		nested_vector += keyword.size();
		cout << "vector";
		shorten_vector_stuff({nested_vector, til_comma.end() - 1});
	}
	else
		cout << til_comma;
	cout << strange{comma, second_comma} << ">";
}

int main(int, char const*const*)
{
	cin >> noskipws;
	string match = "";
	char_ptr i(cin);
	while(i != char_end)
	{
		char current = *i;

		if(current == keyword[match.size()])
		{
			match.push_back(current);
			if(match.size() == keyword.size())
			{
				cout << "vector";
				++i;
				string stuff = "";
				copy_until(i, char_end, back_inserter(stuff), in_scope('<','>'));
				shorten_vector_stuff(make_range(stuff));

				match.clear();
			}
		}
		else
		{
			cout << match << current;
			match.clear();
		}

		++i;
	}
	return 0;
}
