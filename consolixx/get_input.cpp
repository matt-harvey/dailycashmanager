#include "get_input.hpp"
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <boost/exception/all.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/regex.hpp>

using boost::regex;
using boost::regex_match;

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


Decimal get_decimal_from_user()
{
	Decimal ret("0");
	for (bool input_is_valid = false; !input_is_valid; )
	{
		try
		{
			ret = Decimal(get_user_input());
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

boost::gregorian::date
get_date_from_user(string const& error_prompt)
{
	boost::gregorian::date ret = boost::gregorian::day_clock::local_day();
	regex const validation_pattern
	(	"^[0-9][0-9][0-9][0-9][0-1][0-9][0-3][0-9]$"
	);
	while (true)
	{
		string input = get_user_input();
		if (input.empty())
		{
			break;
		}
		if (regex_match(input, validation_pattern))
		{
			try
			{
				ret = boost::gregorian::date_from_iso_string(input);
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
