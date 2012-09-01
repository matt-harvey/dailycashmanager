#ifndef GUARD_commodity_storage_manager_hpp
#define GUARD_commodity_storage_manager_hpp

/** \file commodity_storage_manager.hpp
 *
 * \brief Provides code for managing the storage and retrieval
 * of Commodity object data in and from a sqloxx::DatabaseConnection.
 *
 * \author Matthew Harvey
 * \date 26 Aug 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "commodity.hpp"
#include "database_connection.hpp"
#include "storage_manager.hpp"
#include <jewel/decimal.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace sqloxx
{


class Dummy
{
};

/**
 * Specializes the StorageManager class template for
 * phatbooks::Commodity.
 * 
 * @todo Finish the implementation. Then use this
 * to simplify the code in the PhatbooksDatabaseConnection
 * class. In the end, I may not need PhatbooksDatabaseConnection
 * at all. We'll see...
 */
template <>
class StorageManager<phatbooks::Commodity>
{
public:
	typedef std::string Key;
	static void save(phatbooks::Commodity const& commodity, DatabaseConnection&);
	/**
	 * @todo This needs to throw if there is no Commodity
	 * with this key.
	 */
	static phatbooks::Commodity load(Key const& abbreviation, DatabaseConnection&);
	static void setup_tables(DatabaseConnection&);
};


}  // namespace sqloxx

#endif  // GUARD_commodity_storage_manager_hpp
