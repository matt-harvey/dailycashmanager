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
#include "commodity.hpp"
#include "database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "storage_manager.hpp"

namespace sqloxx
{

/**
 * Specializes the StorageManager class template for
 * phatbooks::Account.
 * 
 * @todo Testing.
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


}  // namespace sqloxx

#endif  // GUARD_account_storage_manager_hpp
