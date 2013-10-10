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


#ifndef GUARD_interval_type_hpp_5555643181109635
#define GUARD_interval_type_hpp_5555643181109635

#include <boost/date_time/gregorian/gregorian.hpp>
#include <wx/string.h>

namespace phatbooks
{

/**
 * Enumerated type representing different
 * date interval.
 *
 * Should be self-explanatory; except note that
 * \c month_ends represents an interval between the
 * last day of one month and the last day of the next,
 * whereas \c months simply represents an interval of
 * one month with no particular reference to the end
 * of the month.
 *
 * Note the numbering of the enumeration is significant.
 * The class PhatbooksDatabaseConnection relies on the
 * numbering shown here.
 */
enum class IntervalType: unsigned char
{
	days = 1,
	weeks,
	months,
	month_ends
};

/**
 * @returns a short phrase describing the interval.
 * E.g. "days", "month", etc.. The phrase is plural if and
 * only if \e is_plural is true.
 */
wxString phrase(IntervalType x, bool is_plural);

/**
 * @returns \e true if and only if \e p_date can occur within
 * an indefinitely long sequence of dates separated by intervals of
 * \e p_interval_type. This is particularly relevant for \e
 * IntervalType::months and \e IntervalType::month_ends. E.g. a date
 * of 29 Feb. 2012 cannot be accommodated by IntervalType::months because
 * not all months have a 29th; and a date of 30 July 2014 cannot be
 * accommodated by IntervalType::month_ends because it is not the last
 * day of a month.
 */
bool is_valid_date_for_interval_type
(	boost::gregorian::date const& p_date,
	IntervalType p_interval_type
);

}  // namespace phatbooks


#endif  // GUARD_interval_type_hpp_5555643181109635
