
/** \file entry.cpp
 *
 * \brief Source file pertaining to Entry class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */




#include "entry.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

using boost::shared_ptr;
using jewel::Decimal;
using std::string;

namespace phatbooks
{

Entry::Entry
(	string const& p_account_name,
	string const& p_comment,
	Decimal const& p_amount
):
	m_account_name(p_account_name),
	m_comment(p_comment),
	m_amount(p_amount)
{
}

string
Entry::comment() const
{
	return m_comment;
}

jewel::Decimal
Entry::amount() const
{
	return m_amount;
}

std::string
Entry::account_name() const
{
	return m_account_name;
}


}  // namespace phatbooks
