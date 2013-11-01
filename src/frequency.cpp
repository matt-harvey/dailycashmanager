/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_conv.hpp"
#include <boost/lexical_cast.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/exception.hpp>
#include <wx/string.h>
#include <string>

using boost::lexical_cast;
using jewel::Decimal;
using jewel::DecimalMultiplicationException;
using std::string;

namespace phatbooks
{


namespace
{
	Decimal const& days_per_year()
	{
		static Decimal const ret("365.25");
		return ret;
	}
	Decimal const& days_per_week()
	{
		static Decimal const ret("7");
		return ret;
	}
	Decimal const& months_per_year()
	{
		static Decimal const ret("12");
		return ret;
	}
	Decimal const& days_per_month()
	{
		static Decimal const ret =
			days_per_year() / months_per_year();
		return ret;
	}
	bool is_whole(Decimal const& d)
	{
		return round(d, 0) == d;
	}
	Decimal const& days_per_canonical_interval()
	{
		// A number with that's wholly divisible
		// by various commonly used Frequencies expressed
		// in numbers of days.
		static Decimal const ret(654528, 0);
		return ret;
	}
	Decimal const& weeks_per_canonical_interval()
	{
		static Decimal const ret =
			days_per_canonical_interval() / days_per_week();
		JEWEL_ASSERT (round(ret, 0) == ret);
		return ret;
	}
	Decimal const& years_per_canonical_interval()
	{
		static Decimal const ret =
			days_per_canonical_interval() / days_per_year();
		JEWEL_ASSERT (round(ret, 0) == ret);
		return ret;
	}
	Decimal const& months_per_canonical_interval()
	{
		static Decimal const ret =
			years_per_canonical_interval() * months_per_year();
		JEWEL_ASSERT (round(ret, 0) == ret);
		return ret;
	}

}  // end anonymous namespace

Frequency::Frequency
(	int p_num_steps,
	IntervalType p_step_type
):
	m_num_steps(p_num_steps),
	m_step_type(p_step_type)
{
	if (p_num_steps < 1)
	{
		JEWEL_THROW
		(	InvalidFrequencyException,
			"In Frequency constructor, p_num_steps passed a value less than 1."
		);
	}
	JEWEL_ASSERT (p_num_steps > 0);
}

int
Frequency::num_steps() const
{
	return m_num_steps;
}

IntervalType
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
		ret += wx_to_std8(phrase(frequency.step_type(), true));
	}
	else
	{
		ret += wx_to_std8(phrase(frequency.step_type(), false));
	}
	return ret;
}

Frequency const&
canonical_frequency()
{
	JEWEL_ASSERT
	(	round(days_per_canonical_interval(), 0) ==
		days_per_canonical_interval()
	);
	static Frequency const ret
	(	round(days_per_canonical_interval(), 0).intval(),
		IntervalType::days
	);
	return ret;
}

Decimal
convert_to_canonical(Frequency const& p_frequency, Decimal const& p_amount)
{
	auto const num_steps = p_frequency.num_steps();
	static_assert
	(	sizeof(Decimal::int_type) >= sizeof(num_steps),
		"Potentially unsafe integral conversion."
	);
	Decimal const steps(num_steps, 0);  // will not throw

	// The next part could throw DecimalMultiplicationException or
	// DecimalDivisionException.
	switch (p_frequency.step_type())
	{
	case IntervalType::days:
		return p_amount * days_per_canonical_interval() / steps;
	case IntervalType::weeks:
		return p_amount * weeks_per_canonical_interval() / steps;
	case IntervalType::months:  // fall through
	case IntervalType::month_ends:
		return p_amount * months_per_canonical_interval() / steps;
	default:
		JEWEL_HARD_ASSERT (false);
	}
}

Decimal
convert_from_canonical(Frequency const& p_frequency, Decimal const& p_amount)
{
	auto const num_steps = p_frequency.num_steps();
	static_assert
	(	sizeof(Decimal::int_type) >= sizeof(num_steps),
		"Potentially unsafe integral conversion."
	);
	Decimal const steps(num_steps, 0);  // will not throw

	// Might throw DecimalMultiplicationException.
	Decimal const intermediate = p_amount * steps;

	// The next part will throw jewel::DecimalDivisionException if and only if
	// the number of significant digits in intermediate is
	// equal to jewel::Decimal::maximum_precision().
	switch (p_frequency.step_type())
	{
	case IntervalType::days:
		return intermediate / days_per_canonical_interval();
	case IntervalType::weeks:
		return intermediate / weeks_per_canonical_interval();
	case IntervalType::months:  // fall through
	case IntervalType::month_ends:
		return intermediate / months_per_canonical_interval();
	default:
		JEWEL_HARD_ASSERT (false);
	}
}

bool
operator==(Frequency const& lhs, Frequency const& rhs)
{
	return
		(lhs.num_steps() == rhs.num_steps()) &&
		(lhs.step_type() == rhs.step_type());
}

bool
operator!=(Frequency const& lhs, Frequency const& rhs)
{
	return !(lhs == rhs);
}

}  // namespace phatbooks
