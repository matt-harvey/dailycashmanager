#include "get_input.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <boost/exception/all.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

namespace gregorian = boost::gregorian;

using boost::function;
using boost::optional;
using boost::regex;
using boost::regex_match;
using jewel::Decimal;
using jewel::DecimalFromStringException;
using jewel::DecimalRangeException;
using std::cout;
using std::istream;
using std::ostream;
using std::string;

namespace consolixx
{


string
get_user_input
(	string const& error_message,
	istream& is,
	ostream& os
)
{
	string input;
	while (!getline(is, input))
	{
		assert (!is);
		is.clear();
		assert (is);
		os << error_message;
		os.flush();
	}
	return input;
}


string
get_constrained_user_input
(	function< bool(string const&) > criterion,
	string const& message_on_invalid_input,
	bool user_can_escape,
	function< bool(string const&) > escape_criterion,
	string const& escape_message,
	string const& return_string_on_escape,
	string const& message_on_unsuccessful_read,
	istream& is,
	ostream& os
)
{
	do
	{
		string input = get_user_input(message_on_unsuccessful_read, is, os);
		if (user_can_escape && escape_criterion(input))
		{
			os << escape_message;
			os.flush();
			return return_string_on_escape;
		}
		if (criterion(input))
		{
			return input;
		}
		assert (!criterion(input));
		os << message_on_invalid_input;
		os.flush();
	}
	while (true);
}


optional<Decimal> get_decimal_from_user(bool allow_enter_to_escape)
{
	optional<Decimal> ret;
	for (bool input_is_valid = false; !input_is_valid; )
	{
		string const input = get_user_input();
		try
		{
			if (allow_enter_to_escape && input.empty())
			{
				return ret;
			}
			ret = Decimal(input);
			input_is_valid = true;
		}
		catch (DecimalFromStringException&)
		{
			assert (!input_is_valid);
			cout << "Please try again, entering a decimal number, "
			     << "(e.g. \"1.343\"): ";
		}
		catch (DecimalRangeException&)
		{
			assert (!input_is_valid);
			cout << "The number you entered is too large or too small to be "
			     << "safely handled. Please try again: ";
		}
	}
	return ret;
}

optional<gregorian::date>
get_date_from_user(bool allow_empty_to_escape, string const& error_prompt)
{
	optional<gregorian::date> ret;
	regex const validation_pattern
	(	"^[0-9][0-9][0-9][0-9][0-1][0-9][0-3][0-9]$"
	);
	while (true)
	{
		string input = get_user_input();
		if (input.empty())
		{
			if (allow_empty_to_escape)
			{
				return ret;
			}
			assert (!allow_empty_to_escape);
		}
		if (regex_match(input, validation_pattern))
		{
			try
			{
				ret = gregorian::date_from_iso_string(input);
				break;
			}
			catch (boost::exception&)
			{
			}
		}
		cout << error_prompt;
	}
	return ret;
}
		

}  // namespace consolixx
