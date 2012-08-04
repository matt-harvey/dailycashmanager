
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
#include <list>
#include <string>

using jewel::Decimal;
using std::list;
using std::string;

namespace phatbooks
{

Journal::Journal(bool p_is_actual):
	m_is_actual(p_is_actual),
	m_is_posted(false),
	m_date((null_date())),
	m_comment(""),
	m_entry_list((list<Entry>())),
	m_repeater_list((list<Repeater>()))
{
}

void
Journal::add_entry
(	string const& account_name,
	string const& entry_comment,
	Decimal const& p_amount
)
{
	m_entry_list.push_back(Entry(account_name, entry_comment, p_amount));
	return;
}
	







}  // namespace phatbooks
