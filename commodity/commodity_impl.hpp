// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_commodity_impl_hpp_9343820716880178
#define GUARD_commodity_impl_hpp_9343820716880178

/** \file commodity_impl.hpp
 *
 * \brief Header file pertaining to CommodityImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "phatbooks_database_connection.hpp"
#include <boost/optional.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <sqloxx/sql_statement_fwd.hpp>
#include <memory>
#include <string>


namespace phatbooks
{


/**
 * Implements Commodity. Multiple Commodity instances may share the same
 * underlying CommodityImpl.
 *
 * @todo Are copy constructor and assignment operator exception-safe?
 */
class CommodityImpl:
	public sqloxx::PersistentObject
	<	CommodityImpl,
		PhatbooksDatabaseConnection
	>
{

public:

	typedef sqloxx::PersistentObject
		<	CommodityImpl,
			PhatbooksDatabaseConnection
		>
		PersistentObject;

	typedef sqloxx::IdentityMap<CommodityImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Return the id of the CommodityImpl with abbreviation p_abbreviation.
	 */
	static sqloxx::Id id_for_abbreviation
	(	PhatbooksDatabaseConnection& dbc,
		wxString const& p_abbreviation
	);

	explicit
	CommodityImpl(IdentityMap& p_identity_map);

	CommodityImpl
	(	IdentityMap& p_identity_map,	
	 	sqloxx::Id p_id
	);

	// copy constructor is private
	
	CommodityImpl(CommodityImpl&&) = delete;
	CommodityImpl& operator=(CommodityImpl const&) = delete;
	CommodityImpl& operator=(CommodityImpl&&) = delete;
	~CommodityImpl() = default;

	static bool exists_with_abbreviation
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_abbreviation
	);

	static bool exists_with_name
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_name
	);

	wxString abbreviation();

	wxString name();

	wxString description();

	int precision();

	jewel::Decimal multiplier_to_base();

	void set_abbreviation(wxString const& p_abbreviation);

	void set_name(wxString const& p_name);

	void set_description(wxString const& p_description);

	void set_precision(int p_precision);

	void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(CommodityImpl& rhs);

	// These need to return std::string as they involve the SQLoxx API
	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();
private:

	/**
	 * Copy constructor - implemented, but deliberately private.
	 */
	CommodityImpl(CommodityImpl const& rhs);

	// Defining pure virtual functions inherited from PersistentObject
	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();

	// Other functions
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct CommodityData;
	std::unique_ptr<CommodityData> m_data;
};

struct CommodityImpl::CommodityData
{
	boost::optional<wxString> abbreviation;
	boost::optional<wxString> name;
	boost::optional<wxString> description;
	boost::optional<int> precision;
	boost::optional<jewel::Decimal> multiplier_to_base;
};



}  // namespace phatbooks

#endif  // GUARD_commodity_impl_hpp_9343820716880178
