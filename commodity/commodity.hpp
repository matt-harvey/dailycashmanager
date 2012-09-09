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
 */
class Commodity:
	public sqloxx::PersistentObject<int>
{

public:

	typedef int Id;
	typedef sqloxx::PersistentObject<Id> PersistentObject;

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

private:

	// Defining pure virtual functions inherited form PersistentObject
	void do_load_all();

	// WARNING I need to implement this properly
	void do_save_existing_all()
	{
	}

	// WARNING I need to implement this properly
	void do_save_existing_partial()
	{
	}


	void do_save_new_all();

	std::string do_get_table_name();

	// Other functions
	void load_abbreviation_knowing_id();
	void load_id_knowing_abbreviation();

	// Data members. The non-optional ones are initialized
	// by every constructor; and the optional ones are
	// only initialized on full load.
	std::string m_abbreviation;
	boost::optional<std::string> m_name;
	boost::optional<std::string> m_description;
	boost::optional<int> m_precision;
	boost::optional<jewel::Decimal> m_multiplier_to_base;
};


// Inline member functions


inline
Commodity::Commodity
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection
):
	PersistentObject(p_database_connection)
{
}

inline
Commodity::Commodity
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	Id p_id
):
	PersistentObject(p_database_connection, p_id)
{
	load_abbreviation_knowing_id();
}

inline
Commodity::Commodity
(	boost::shared_ptr<sqloxx::DatabaseConnection> p_database_connection,
	std::string const& p_abbreviation
):
	PersistentObject(p_database_connection),
	m_abbreviation(p_abbreviation)
{
	load_id_knowing_abbreviation();
	mark_as_persisted();
}
	
inline
std::string Commodity::abbreviation()
{
	// load not done as m_abbreviation is always initialized.
	return m_abbreviation;
}

inline
std::string Commodity::name()
{
	load();
	return *m_name;
}

inline
std::string Commodity::description()
{
	load();
	return *m_description;
}

inline
int Commodity::precision()
{
	load();
	return *m_precision;
}

inline
jewel::Decimal Commodity::multiplier_to_base()
{
	load();
	return *m_multiplier_to_base;
}

inline
void Commodity::set_abbreviation(std::string const& p_abbreviation)
{
	m_abbreviation = p_abbreviation;
	return;
}

inline
void Commodity::set_name(std::string const& p_name)
{
	m_name = p_name;
	return;
}

inline
void Commodity::set_description(std::string const& p_description)
{
	m_description = p_description;
	return;
}

inline
void Commodity::set_precision(int p_precision)
{
	m_precision = p_precision;
	return;
}

inline
void Commodity::set_multiplier_to_base
(	jewel::Decimal const& p_multiplier_to_base
)
{
	m_multiplier_to_base = p_multiplier_to_base;
	return;
}

inline
std::string Commodity::do_get_table_name()
{
	return "commodities";
}











}  // namespace phatbooks

#endif  // GUARD_commodity_hpp
