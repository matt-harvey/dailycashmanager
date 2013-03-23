#include "frequency.hpp"
#include "interval_type.hpp"
#include <boost/lexical_cast.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <string>

using boost::lexical_cast;
using jewel::Decimal;
using jewel::DecimalMultiplicationException;
using std::string;

namespace phatbooks
{

Frequency::Frequency
(	int p_num_steps,
	interval_type::IntervalType p_step_type
):
	m_num_steps(p_num_steps),
	m_step_type(p_step_type)
{
}

int
Frequency::num_steps() const
{
	return m_num_steps;
}

interval_type::IntervalType
Frequency::step_type() const
{
	return m_step_type;
}

string
frequency_description(Frequency const& frequency, string const& first_word)
{
	string ret = first_word + " ";
	int const num_steps = frequency.num_steps();
	if (num_steps > 1)
    {
		ret += lexical_cast<string>(num_steps);
		ret += " ";
		ret += bstring_to_std8(phrase(frequency.step_type(), true));
	}
	else
	{
		ret += bstring_to_std8(phrase(frequency.step_type(), false));
	}
	return ret;
}



namespace
{
	Decimal const days_per_year()
	{
		static Decimal const ret("365.25");
		return ret;
	}
	Decimal const days_per_week()
	{
		static Decimal const ret("7");
		return ret;
	}
	Decimal const months_per_year()
	{
		static Decimal const ret("12");
		return ret;
	}
	Decimal const days_per_month()
	{
		static Decimal const ret =
			days_per_year() / months_per_year();
		return ret;
	}
	bool is_whole(Decimal const& d)
	{
		return round(d, 0) == d;
	}
	Decimal const days_per_canonical_interval()
	{
		// A number with that's wholly divisible
		// by various commonly used Frequencies expressed
		// in numbers of days.
		static Decimal const ret(654528, 0);
		return ret;
	}
	Decimal const weeks_per_canonical_interval()
	{
		static Decimal const ret =
			days_per_canonical_interval() / days_per_week();
		assert (round(ret, 0) == ret);
		return ret;
	}
	Decimal const years_per_canonical_interval()
	{
		static Decimal const ret =
			days_per_canonical_interval() / days_per_year();
		assert (round(ret, 0) == ret);
		return ret;
	}
	Decimal const months_per_canonical_interval()
	{
		static Decimal const ret =
			years_per_canonical_interval() * months_per_year();
		assert (round(ret, 0) == ret);
		return ret;
	}

}  // end anonymous namespace


Frequency const canonical_frequency()
{
	assert
	(	round(days_per_canonical_interval(), 0) ==
		days_per_canonical_interval()
	);
	static Frequency const ret
	(	round(days_per_canonical_interval(), 0).intval(),
		interval_type::days
	);
	return ret;
}

Decimal
convert_to_annual(Frequency const& p_frequency, Decimal const& p_amount)
{
	Decimal const steps(p_frequency.num_steps(), 0);
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_amount * days_per_year() / steps;
	case interval_type::weeks:
		return p_amount * days_per_year() / days_per_week() / steps;
	case interval_type::months:  // fall through
	case interval_type::month_ends:
		return p_amount * months_per_year() / steps;
	default:
		assert (false);
	}
}

Decimal
convert_from_annual(Frequency const& p_frequency, Decimal const& p_amount)
{
	Decimal const steps(p_frequency.num_steps(), 0);
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_amount * steps / days_per_year();
	case interval_type::weeks:
		return p_amount * steps * days_per_week() / days_per_year();
	case interval_type::months:  // fall through
	case interval_type::month_ends:
		return p_amount	* steps / months_per_year();
	default:
		assert (false);
	}
}

Decimal
convert_to_canonical(Frequency const& p_frequency, Decimal const& p_amount)
{
	Decimal const steps(p_frequency.num_steps(), 0);
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_amount * days_per_canonical_interval() / steps;
	case interval_type::weeks:
		return p_amount * weeks_per_canonical_interval() / steps;
	case interval_type::months:  // fall through
	case interval_type::month_ends:
		return p_amount * months_per_canonical_interval() / steps;
	default:
		assert (false);
	}
}

Decimal
convert_from_canonical(Frequency const& p_frequency, Decimal const& p_amount)
{
	Decimal const steps(p_frequency.num_steps(), 0);
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_amount * steps / days_per_canonical_interval();
	case interval_type::weeks:
		return p_amount * steps / weeks_per_canonical_interval();
	case interval_type::months:  // fall through
	case interval_type::month_ends:
		return p_amount * steps / months_per_canonical_interval();
	default:
		assert (false);
	}
}

// TODO How to handle DecimalMultiplicationException and
// DecimalDivisionException in the Frequency convertions
// functions? Will they occur often?
// The answer may be just to force the user not to input weird Frequencies
// that we know could cause issues.


}  // namespace phatbooks
