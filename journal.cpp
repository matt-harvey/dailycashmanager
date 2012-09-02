
/** \file journal.cpp
 *
 * \brief Source file relating to Journal class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "journal.hpp"
#include "date.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "entry.hpp"
#include "repeater.hpp"
#include <jewel/decimal.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>

using boost::shared_ptr;
using jewel::Decimal;
using std::list;
using std::string;

namespace phatbooks
{

Journal::Journal(bool p_is_actual, string p_comment):
	m_is_actual(p_is_actual),
	m_date((null_date())),
	m_comment(p_comment),
	m_entries(list< shared_ptr<Entry> >()),
	m_repeaters(list< shared_ptr<Repeater> >())
{
}

void
Journal::set_whether_actual(bool p_is_actual)
{
	m_is_actual = p_is_actual;
	return;
}

void
Journal::set_comment(string const& p_comment)
{
	m_comment = p_comment;
	return;
}

void
Journal::set_date(DateType date)
{
	m_date = date;
	return;
}

void
Journal::add_entry(shared_ptr<Entry> entry)
{
	m_entries.push_back(entry);
	return;
}

void
Journal::add_repeater(shared_ptr<Repeater> repeater)
{
	m_repeaters.push_back(repeater);
	return;
}

bool
Journal::is_posted() const
{
	return m_date != null_date();
}

bool
Journal::is_actual() const
{
	return m_is_actual;
}

DateType
Journal::date() const
{
	return m_date;
}

string
Journal::comment() const
{
	return m_comment;
}





}  // namespace phatbooks
