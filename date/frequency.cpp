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
frequency_description(Frequency const& frequency)
{
	string ret = "every ";
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

}  // End anonymous namespace


Decimal
convert_to_annual(Frequency const& p_frequency, Decimal const& p_amount)
{
	// TODO How to handle DecimalMultiplicationException and
	// DecimalDivisionException? Will they occur often?
	// We could get all complicatad in the below to do rounding
	// to prevent overflow in many cases - but that complexity probably
	// belongs in jewel::Decimal, not here...
	Decimal const steps(p_frequency.num_steps(), 0);
	switch (p_frequency.step_type())
	{
	case interval_type::days:
		return p_amount * days_per_year() / steps;
	case interval_type::weeks:
		return p_amount * days_per_year() / days_per_week() / steps;
	case interval_type::months: // Fall through
	case interval_type::month_ends:
		return p_amount * months_per_year() / steps;
	default:
		assert (false);
	}
}
	

Decimal
convert_from_annual(Frequency const& p_frequency, Decimal const& p_amount)
{
	// TODO How to handle DecimalMultiplicationException and
	// DecimalDivisionException? Will they occur often?
	// We could get all complicatad in the below to do rounding
	// to prevent overflow in many cases - but that complexity probably
	// belongs in jewel::Decimal, not here...
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


}  // namespace phatbooks
