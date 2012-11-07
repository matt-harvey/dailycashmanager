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


#include "general_typedefs.hpp"
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <sqloxx/database_connection.hpp>
#include <sqloxx/persistent_object.hpp>
#include <string>

namespace phatbooks
{

/**
 * Class representing commodities, where a commodity is anything of
 * value that can be counted in undifferentiated units, e.g. a particular
 * currency, units in a particular fund, shares in a particular trust,
 * and so forth. A single unique asset, such as a car, might also be
 * regarded as a commodity, of which there is only one unit in
 * existence.
 *
 * @todo Are copy constructor and assignment operator exception-safe?
 */
class Commodity: public sqloxx::PersistentObject
{

public:

	typedef sqloxx::PersistentObject PersistentObject;
	typedef PersistentObject::Id Id;

	/**
	 * Sets up tables required in the database for the persistence
	 * of Commodity objects.
	 */
	static void setup_tables(sqloxx::DatabaseConnection& dbc);

	/**
	 * Initialize a "draft" commodity, that will not correspond to
	 * any particular object in the database.
	 */
	explicit
	Commodity
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
	);

	/**
	 * Get a Commodity by id from database.
	 */
	Commodity
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	 	Id p_id
	);

	/**
	 * Get a Commodity by abbreviation from database
	 */
	Commodity
	(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
		std::string const& p_abbreviation
	);

	 // Use default destructor.

	
	/**
	 * Get the abbreviation of the commodity. E.g "AUD" might be
	 * the abbreviation for Australian dollars, or "NAB.AX" might be
	 * the abbreviation for ordinary stock in National Australia Bank
	 * Limited.
	 */
	std::string abbreviation();

	/**
	 * Get the full name of the commodity. E.g. "Australian dollars".
	 */
	std::string name();

	/**
	 * Get the description of the commodity.
	 */
	std::string description();

	/**
	 * Get the number of decimal places of precision for the commodity
	 * to which quantities of the commodity are stored.
	 */
	int precision();

	/**
	 * Get the multiplier by which 1 unit of the commodity should
	 * be mulitiplied, to be translated into 1 unit of the base commodity
	 * of the entity.
	 */
	jewel::Decimal multiplier_to_base();

	void set_abbreviation(std::string const& p_abbreviation);

	void set_name(std::string const& p_name);

	void set_description(std::string const& p_description);

	void set_precision(int p_precision);

	void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(Commodity& rhs);

private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	Commodity(Commodity const& rhs);

	// Defining pure virtual functions inherited from PersistentObject
	virtual void do_load_all();

	// WARNING I need to implement this properly
	virtual void do_save_existing_all()
	{
	}

	// WARNING I need to implement this properly
	virtual void do_save_existing_partial()
	{
	}

	virtual void do_save_new_all();

	virtual std::string do_get_table_name() const;

	// Other functions
	void load_abbreviation_knowing_id();
	void load_id_knowing_abbreviation();

	struct CommodityData
	{
		// Data members. The non-optional ones are initialized
		// by every constructor; and the optional ones are
		// only initialized on full load.
		std::string abbreviation;
		boost::optional<std::string> name;
		boost::optional<std::string> description;
		boost::optional<int> precision;
		boost::optional<jewel::Decimal> multiplier_to_base;
	};

	boost::shared_ptr<CommodityData> m_data;
};




}  // namespace phatbooks

#endif  // GUARD_commodity_hpp
