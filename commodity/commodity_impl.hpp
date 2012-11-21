#ifndef GUARD_commodity_impl_hpp
#define GUARD_commodity_impl_hpp

/** \file commodity_impl.hpp
 *
 * \brief Header file pertaining to CommodityImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/optional.hpp>
#include <sqloxx/persistent_object.hpp>
#include <string>


namespace sqloxx
{
	class SharedSQLStatement;
}  // namespace sqloxx


namespace phatbooks
{

class PhatbooksDatabaseConnection;

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
class CommodityImpl:
	public sqloxx::PersistentObject<CommodityImpl, PhatbooksDatabaseConnection>
{

public:

	typedef sqloxx::PersistentObject<CommodityImpl, PhatbooksDatabaseConnection>
		PersistentObject;
	typedef sqloxx::Id Id;

	/**
	 * Sets up tables required in the database for the persistence
	 * of CommodityImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Return the id of the CommodityImpl with abbreviation p_abbreviation.
	 */
	static Id id_for_abbreviation
	(	PhatbooksDatabaseConnection& dbc,
		std::string const& p_abbreviation
	);

	/**
	 * Initialize a "draft" commodity, that will not correspond to
	 * any particular object in the database.
	 */
	explicit
	CommodityImpl
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection
	);

	/**
	 * Get a CommodityImpl by id from database.
	 */
	CommodityImpl
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection,
	 	Id p_id
	);


	/**
	 * Destructor
	 */
	~CommodityImpl();
	
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
	void swap(CommodityImpl& rhs);

	static std::string primary_table_name();
private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	CommodityImpl(CommodityImpl const& rhs);

	// Defining pure virtual functions inherited from PersistentObject
	void do_load();
	void do_save_existing();
	void do_save_new();

	// Other functions
	void load_abbreviation_knowing_id();
	void process_saving_statement(sqloxx::SharedSQLStatement& statement);

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

	boost::scoped_ptr<CommodityData> m_data;
};




}  // namespace phatbooks

#endif  // GUARD_commodity_impl_hpp
