#ifndef GUARD_journal_storage_manager_hpp
#define GUARD_journal_storage_manager_hpp

/** \file journal_storage_manager.hpp
 *
 * \brief Provides code for managing storage and
 * retrieval of phatbooks::Journal object data in and from a
 * sqloxx::DatabaseConnection.
 *
 * \author Matthew Harvey
 * \date 02 Sep 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "journal.hpp"
#include "general_typedefs.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/storage_manager.hpp"
#include <jewel/decimal.hpp>

namespace sqloxx
{

/**
 * Specializes the StorageManager class template for
 * phatbooks::Account.
 *
 * @todo This currently doesn't store any repeaters.
 *
 * @todo This currently assumes the Journal is not a draft journal.
 */
template <>
class StorageManager<phatbooks::Journal>
{
public:
	
	typedef phatbooks::IdType Key;

	static void
	save(phatbooks::Journal const& journal, DatabaseConnection& dbc);

	/**
	 * @todo This needs to throw if there is no Journal with this key.
	 *
	 * @todo I don't like the way the implementation of this
	 * uses the accounts_extended table - which is buried
	 * in StorageManager<Account>::setup_tables. It is relying, therefore,
	 * on the innards of another class. But it's also much more efficient
	 * to use this.
	 */
	static phatbooks::Journal
	load(Key const& id, DatabaseConnection& dbc);

	static void
	setup_tables(DatabaseConnection& dbc);

};

}  // namespace sqloxx

#endif  // GUARD_journal_storage_manager_hpp
