#ifndef GUARD_account_storage_manager_hpp
#define GUARD_account_storage_manager_hpp

/** \file account_storage_manager.hpp
 *
 * \brief Provides code for managing the storage and retrieval
 * of Account object data in and from a sqloxx::DatabaseConnection.
 *
 * \author Matthew Harvey
 * \date 26 Aug 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "account.hpp"
#include "database_connection.hpp"
#include "storage_manager.hpp"

namespace sqloxx
{

/**
 * Specializes the StorageManager class template for
 * phatbooks::Account.
 * 
 * @todo Finish the implementation. Then use this
 * to simplify the code in the PhatbooksDatabaseConnection
 * class. In the end, I may not need PhatbooksDatabaseConnection
 * at all. We'll see...
 */
template <>
class StorageManager<phatbooks::Account>
{
public:
	typedef std::string Key;
	static void save(phatbooks::Account const& account, DatabaseConnection& db);
	/**
	 * @todo This needs to throw if there is no Account
	 * with this key.
	 */
	static phatbooks::Account load(Key const& name, DatabaseConnection& db);
	static void setup_tables(DatabaseConnection& db);
};


inline
void StorageManager<phatbooks::Account>::setup_tables
(	DatabaseConnection& dbc
)
{
	typedef DatabaseConnection::SQLStatement Statement;
	Statement account_types_table_stmt
	(	dbc,
		"create table account_types(account_type_id integer primary key "
		"autoincrement, name text not null unique)"
	);
	account_types_table_stmt.quick_step();

	// Values inserted into account_types here must correspond with
	// AccountType enum defined in Account class
	std::vector<std::string> const names =
		phatbooks::Account::account_type_names();
	for (std::vector<std::string>::size_type i = 0; i != names.size(); ++i)
	{
		std::string const str =
			"insert into account_types(name) values('" + names[i] + "')";
		Statement populator(dbc, str);
		populator.quick_step();
	}
	Statement accounts_table_stmt
	(	dbc,
		"create table accounts "
		"("
			"account_id integer primary key autoincrement, "
			"account_type_id not null references account_types, "
			"name text not null unique, "
			"description text, "
			"commodity_id references commodities"
		")"
	);
	accounts_table_stmt.quick_step();
	return;
}



}  // namespace sqloxx

#endif  // GUARD_account_storage_manager_hpp
