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
#include "database_connection.hpp"
#include "storage_manager.hpp"
#include <jewel/decimal.hpp>

namespace sqloxx
{

/**
 * Specializes the StorageManager class template for
 * phatbooks::Account.
 */
template <>
class StorageManager<phatbooks::Journal> {
public:
	
	typedef phatbooks::IdType Key;

	static void
	save(phatbooks::Journal const& journal, DatabaseConnection& dbc);

	/**
	 * @todo This needs to throw if there is no Journal with this key.
	 */
	static phatbooks::Journal
	load(Key const& id, DatabaseConnection& dbc);

	static void
	setup_tables(DatabaseConnection& dbc);

};


}  // namespace sqloxx

#endif  // GUARD_journal_storage_manager_hpp
