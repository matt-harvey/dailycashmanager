#include "commodity.hpp"
#include <jewel/decimal.hpp>
#include <string>

/** \file commodity.cpp
 *
 * \brief Source file pertaining to Commodity class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


using jewel::Decimal;
using std::string;

namespace phatbooks
{

Commodity::Commodity
(	std::string p_abbreviation,
	std::string p_name,
	std::string p_description,
	int p_precision,
	jewel::Decimal p_multiplier_to_base
):
	m_abbreviation(p_abbreviation),
	m_name(p_name),
	m_description(p_description),
	m_precision(p_precision),
	m_multiplier_to_base(p_multiplier_to_base)
{
}

string
Commodity::abbreviation() const
{
	return m_abbreviation;
}


string
Commodity::name() const
{
	return m_name;
}


string
Commodity::description() const
{
	return m_description;
}


int
Commodity::precision() const
{
	return m_precision;
}


Decimal
Commodity::multiplier_to_base() const
{
	return m_multiplier_to_base;
}


}  // namespace phatbooks
