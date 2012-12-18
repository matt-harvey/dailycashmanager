#include "finformat.hpp"
#include <jewel/decimal.hpp>
#include <cassert>
#include <locale>
#include <ostream>
#include <string>

using jewel::Decimal;
using std::locale;
using std::ostringstream;
using std::string;

namespace phatbooks
{


string finformat(Decimal const& decimal)
{
	static Decimal const zero = Decimal(0, 0);
	ostringstream oss;
	oss.imbue(locale(""));
	oss << decimal;
	string ret(oss.str());
	if (ret[0] == '-')
	{
		assert (decimal < zero);
		ret[0] = '(';
		ret.push_back(')');
	}
	else if (decimal > zero)
	{
		ret.push_back(' ');
	}
	else if (decimal == zero)
	{
		ret = "-" + string(decimal.places() + 1, ' ');
	}
	else
	{
		assert (false);
	}
	return ret;
}


}  // namespace phatbooks

