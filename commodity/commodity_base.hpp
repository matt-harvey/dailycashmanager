#ifndef GUARD_commodity_base_hpp
#define GUARD_commodity_base_hpp

/** \file commodity_base.hpp
 *
 * \brief Header file pertaining to CommodityBase class.
 *
 * \author Matthew Harvey
 * \date 04 Sep 2012
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "general_typedefs.hpp"
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

/**
 * Abstract base class providing interface to commodities, where a commodity
 * is anything of
 * value that can be counted in undifferentiated units, e.g. a particular
 * currency, units in a particular fund, shares in a particular trust, and
 * so forth. A single unique asset, such as a car, might also be regarded as a
 * commodity, of which there is only one unit in existence.
 */
class CommodityBase
{

public:

	/**
	 * Get the abbreviation of the commodity.
	 */
	virtual std::string abbreviation() const = 0;

	/**
	 * Get the name of the commodity.
	 */
	virtual std::string name() const = 0;

	/**
	 * Get the description of the commodity.
	 */
	virtual std::string description() const = 0;

	/**
	 * Get the number of decimal places of precision for the commodity
	 * to which quantities of the commodity are stored.
	 */
	virtual int precision() const = 0;

	/**
	 * Get the multiplier by which 1 unit of the commodity should
	 * be mulitiplied, to be translated into 1 unit of the base commodity
	 * of the entity.
	 */
	virtual jewel::Decimal multiplier_to_base() const = 0;

};



}  // namespace phatbooks

#endif  // GUARD_commodity_base_hpp
