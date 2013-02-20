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


#include "phatbooks_database_connection.hpp"
#include <sqloxx/identity_map.hpp>
#include <sqloxx/persistent_object.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/optional.hpp>
#include <string>


namespace sqloxx
{
	class SQLStatement;
}  // namespace sqloxx


namespace phatbooks
{


/**
 * Implements Commodity. Multiple Commodity instances may share the same
 * underlying CommodityImpl.
 *
 * @todo Are copy constructor and assignment operator exception-safe?
 */
class CommodityImpl:
	public sqloxx::PersistentObject<CommodityImpl, PhatbooksDatabaseConnection>
{

public:

	typedef sqloxx::PersistentObject
		<	CommodityImpl,
			PhatbooksDatabaseConnection
		>
		PersistentObject;
	typedef sqloxx::Id Id;
	typedef sqloxx::IdentityMap<CommodityImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Return the id of the CommodityImpl with abbreviation p_abbreviation.
	 */
	static Id id_for_abbreviation
	(	PhatbooksDatabaseConnection& dbc,
		BString const& p_abbreviation
	);

	explicit
	CommodityImpl(IdentityMap& p_identity_map);

		CommodityImpl
	(	IdentityMap& p_identity_map,	
	 	Id p_id
	);

	~CommodityImpl();
	
	BString abbreviation();

	BString name();

	BString description();

	int precision();

	jewel::Decimal multiplier_to_base();

	void set_abbreviation(BString const& p_abbreviation);

	void set_name(BString const& p_name);

	void set_description(BString const& p_description);

	void set_precision(int p_precision);

	void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

	/**
	 * @todo Provide non-member swap and specialized std::swap per
	 * "Effective C++".
	 */
	void swap(CommodityImpl& rhs);

	// These need to return std::string as they involve the SQLoxx API
	static std::string primary_table_name();
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
	boost::scoped_ptr<CommodityData> m_data;
};

struct CommodityImpl::CommodityData
{
	boost::optional<BString> abbreviation;
	boost::optional<BString> name;
	boost::optional<BString> description;
	boost::optional<int> precision;
	boost::optional<jewel::Decimal> multiplier_to_base;
};



}  // namespace phatbooks

#endif  // GUARD_commodity_impl_hpp
