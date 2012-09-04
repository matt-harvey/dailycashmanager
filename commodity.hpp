#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

/** \file commodity.hpp
 *
 * \brief Header file pertaining to Commodity class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "commodity_base.hpp"
#include "general_typedefs.hpp"
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

/**
 * Class representing Commodity objects that are "live" in memory,
 * rather than stored in a database.
 */
class Commodity: public CommodityBase
{

public:

	/**
	 * @param p_abbreviation abbreviation for commodity, e.g. "AUD" might be
	 * used as the abbreviation for Australian dollars, or "NAB.AX" as the
	 * abbreviation for ordinary stock in National Australia Bank Limited.
	 *
	 * @param p_name Full name of the commodity, e.g. "Australian dollars"
	 *
	 * @param p_description Description of the commodity.
	 *
	 * @param p_precision Describes the number of decimal places of precision
	 * to which this commodity should be stored. This affects how quantities
	 * of the commodity will be stored in the database.
	 *
	 * @param p_multiplier_to_base Where the "base" is the base commodity of
	 * the accounting entity, \c p_multiplier_to_base represents the number
	 * by which a quantity of the commodity should be multiplied, in order
	 * to convert it to one unit of the base commodity. (Note a currency is
	 * just a commodity; although the notion of a commodity is broader, and
	 * could also include, for example, shares, or units in a trust.)
	 */
	Commodity
	(	std::string p_abbreviation,
		std::string p_name = "",
		std::string p_description = "",
		int p_precision = 2,
		jewel::Decimal p_multiplier_to_base = jewel::Decimal("1")
	);
	
	/**
	 * Get the abbreviation of the commodity.
	 */
	std::string abbreviation() const;

	/**
	 * Get the name of the commodity.
	 */
	std::string name() const;

	/**
	 * Get the description of the commodity.
	 */
	std::string description() const;

	/**
	 * Get the number of decimal places of precision for the commodity
	 * to which quantities of the commodity are stored.
	 */
	int precision() const;

	/**
	 * Get the multiplier by which 1 unit of the commodity should
	 * be mulitiplied, to be translated into 1 unit of the base commodity
	 * of the entity.
	 */
	jewel::Decimal multiplier_to_base() const;

private:
	std::string m_abbreviation;
	std::string m_name;
	std::string m_description;
	int m_precision;
	jewel::Decimal m_multiplier_to_base;
};



}  // namespace phatbooks

#endif  // GUARD_commodity_hpp
