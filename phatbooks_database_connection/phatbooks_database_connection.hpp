#ifndef GUARD_phatbooks_database_connection_hpp
#define GUARD_phatbooks_database_connection_hpp

/** \file phatbooks_database_connection.hpp
 *
 * \brief Header file pertaining to PhatbooksDatabaseConnection class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account_impl.hpp"
#include "commodity_impl.hpp"
#include "draft_journal.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "repeater.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/identity_map.hpp"
#include <boost/bimap.hpp>
#include <string>

namespace phatbooks
{

// Forward declarations
class Journal;


/** Phatbooks-specific database connection class.
 * See API documentation for sqloxx::DatabaseConnection,
 * for parts of API inherited from sqloxx::DatabaseConnection.
 */
class PhatbooksDatabaseConnection:
	public sqloxx::DatabaseConnection
{
public:
	/**
	 * Exhibits the same throwing behaviour (if any) as
	 * default constructor for sqloxx::DatabaseConnection.
	 */
	PhatbooksDatabaseConnection();

	/**
	 * @returns \c true if and only if \c p_name is the name of an Account
	 * stored in the database.
	 *
	 * @param p_name name of Account.
	 *
	 * @throws SQLiteException or an exception derived therefrom, if
	 * something goes wrong with the SQL involved in executing
	 * the function. This might occur if \c p_name contains punctuation
	 * marks, spaces or etc., or is a SQL keyword, so that it cannot validly
	 * be a string parameter in a SQL statement. A SQLiteException or
	 * derivative might also be thrown if the database connection is invalid
	 * or the database corrupt. It is not guaranteed that these are the only
	 * circumstances in which an exception might be thrown.
	 */
	bool has_account_named(std::string const& p_name);

	/**
	 * @returns \c true if and only if \c p_name is the name of a
	 * DraftJournal stored in the database.
	 *
	 * @todo Document throwing behaviour.
	 */
	bool has_draft_journal_named(std::string const& p_name);
	

	/**
	 * @returns a boost::bimap from AccountType to string, giving the
	 * string name for each AccountType.
	 *
	 * @todo Determine and document throwing behaviour.
	 */
	boost::bimap<account_type::AccountType, std::string> account_types();

	/**
	 * @returns \c true if and only if \c p_abbreviation is the abbreviation
	 * of a Commodity stored in the database.
	 */
	bool has_commodity_with_abbreviation(std::string const& p_abbreviation);

	/**
	 * @returns \c true if and only if \c p_name is the name of a Commodity
	 * stored in the database.
	 */
	bool has_commodity_named(std::string const& p_name);

	/**
	 * Creates tables required for Phatbooks, and inserts rows
	 * into certain tables to provide application-level data where
	 * required. If the database already contains these tables,
	 * the function does nothing. This function should always be
	 * called after calling DatabaseConnection::open.
	 *
	 * @throws SQLiteException or some derivative thereof, if setup is
	 * unsuccessful.
	 */
	void setup();

	sqloxx::IdentityMap<AccountImpl>& account_map();
	sqloxx::IdentityMap<CommodityImpl>& commodity_map();
	sqloxx::IdentityMap<EntryImpl>& entry_map();
	sqloxx::IdentityMap<OrdinaryJournal>& ordinary_journal_map();
	sqloxx::IdentityMap<DraftJournal>& draft_journal_map();
	sqloxx::IdentityMap<Repeater>& repeater_map();

private:

	bool setup_has_occurred();
	void mark_setup_as_having_occurred();


	sqloxx::IdentityMap<AccountImpl> m_account_map;
	sqloxx::IdentityMap<CommodityImpl> m_commodity_map;
	sqloxx::IdentityMap<EntryImpl> m_entry_map;
	sqloxx::IdentityMap<OrdinaryJournal> m_ordinary_journal_map;
	sqloxx::IdentityMap<DraftJournal> m_draft_journal_map;
	sqloxx::IdentityMap<Repeater> m_repeater_map;



};  // PhatbooksDatabaseConnection


}  // namespace phatbooks



namespace sqloxx
{

template <>
inline
IdentityMap<phatbooks::AccountImpl>&
identity_map
<	phatbooks::AccountImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.account_map();
}

template <>
inline
IdentityMap<phatbooks::EntryImpl>&
identity_map
<	phatbooks::EntryImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.entry_map();
}

template <>
inline
IdentityMap<phatbooks::CommodityImpl>&
identity_map
<	phatbooks::CommodityImpl,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.commodity_map();
}

template <>
inline
IdentityMap<phatbooks::OrdinaryJournal>&
identity_map
<	phatbooks::OrdinaryJournal,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.ordinary_journal_map();
}

template <>
inline
IdentityMap<phatbooks::DraftJournal>&
identity_map
<	phatbooks::DraftJournal,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.draft_journal_map();
}

template <>
inline
IdentityMap<phatbooks::Repeater>&
identity_map
<	phatbooks::Repeater,
	phatbooks::PhatbooksDatabaseConnection
>
(	phatbooks::PhatbooksDatabaseConnection& connection
)
{
	return connection.repeater_map();
}

}  // namespace sqloxx




#endif  // GUARD_phatbooks_database_connection_hpp
