// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_commodity_hpp_9343820716880178
#define GUARD_commodity_hpp_9343820716880178

/** \file commodity.hpp
 *
 * \brief Header file pertaining to Commodity class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "phatbooks_database_connection.hpp"
#include <jewel/log.hpp>
#include <jewel/decimal_fwd.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <memory>
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
 * Client code must deal with sqloxx::Handle<Commodity> rather than with
 * Commodity instances directly. This is enforced via "Signature"
 * parameters in the constructors for Commodity, which make it
 * impossible for client code to call these constructors directly.
 *
 * @todo The Commodity funcionality of Phatbooks should for the time
 * being remain completely hidden from the user - mainly because it
 * isn't properly implemented yet, because we can't deal with the
 * situation in which there is more than one commodity in the database.
 * That part of the Commodity functionality that has already been
 * written, is for the time being being hidden by the preprocessor.
 * To unhide it, define PHATBOOKS_EXPOSE_COMMODITY.
 *
 * @todo Are copy constructor and assignment operator exception-safe?
 */
class Commodity:
	public sqloxx::PersistentObject
	<	Commodity,
		PhatbooksDatabaseConnection
	>
{

public:

	typedef sqloxx::PersistentObject
		<	Commodity,
			PhatbooksDatabaseConnection
		>
		PersistentObject;

	typedef sqloxx::IdentityMap<Commodity> IdentityMap;

	/**
	 * Set up tables required in the database for the persistence of
	 * Commodity objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Return the id of the Commodity with abbreviation p_abbreviation.
	 */
	static sqloxx::Id id_for_abbreviation
	(	PhatbooksDatabaseConnection& dbc,
		wxString const& p_abbreviation
	);

	/**
	 * Construct a fresh Commodity, not yet persisted to the database.
	 *
	 * Client code cannot call this constructor, as it is solely for
	 * use by IdentityMap. This is enforced by the Signature parameter.
	 */
	Commodity
	(	IdentityMap& p_identity_map,
		IdentityMap::Signature const& p_signature
	);

	/**
	 * Construct Commodity already persisted to database with Id of
	 * p_id.
	 *
	 * Client code cannot call this constructor, as it is solely
	 * for use by IdentityMap. This is enforced by the Signature
	 * parameter.
	 */
	Commodity
	(	IdentityMap& p_identity_map,	
	 	sqloxx::Id p_id,
		IdentityMap::Signature const& p_signature
	);

	// copy constructor is private
	
	Commodity(Commodity&&) = delete;
	Commodity& operator=(Commodity const&) = delete;
	Commodity& operator=(Commodity&&) = delete;
	~Commodity();

	/**
	 * @returns \c true if and only if \c p_abbreviation is the abbreviation
	 * of a Commodity stored in the database.
	 */
	static bool exists_with_abbreviation
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_abbreviation
	);

	/**
	 * @returns \c true if and only if \c p_name is the name of a Commodity
	 * stored in the database.
	 */
	static bool exists_with_name
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_name
	);

	/**
	 * Get the abbreviation of the commodity. E.g "AUD" might be
	 * the abbreviation for Australian dollars.
	 */
	wxString abbreviation();

	/**
	 * Get the full name of the commodity. E.g. "Australian dollars".
	 */
	wxString name();

	/**
	 * Get the description of the commodity, e.g. "notes and coins".
	 */
	wxString description();

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

	void set_abbreviation(wxString const& p_abbreviation);

	void set_name(wxString const& p_name);

	void set_description(wxString const& p_description);

	void set_precision(int p_precision);

	void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

	// These need to return std::string as they involve the SQLoxx API
	static std::string exclusive_table_name();
	static std::string primary_key_name();

private:

	void swap(Commodity& rhs);

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	Commodity(Commodity const& rhs);

	// Defining pure virtual functions inherited from PersistentObject
	void do_load() override;
	void do_save_existing() override;
	void do_save_new() override;
	void do_ghostify() override;

	// Other functions
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct CommodityData;
	std::unique_ptr<CommodityData> m_data;
};

}  // namespace phatbooks

#endif  // GUARD_commodity_hpp_9343820716880178
