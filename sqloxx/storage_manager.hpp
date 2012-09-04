#ifndef GUARD_storage_manager_hpp
#define GUARD_storage_manager_hpp

/** \file storage_manager.hpp
 *
 * \brief Provides template code for managing the storage and retrieval
 * of object data in and from a sqloxx::DatabaseConnection.
 *
 * \author Matthew Harvey
 * \date 26 Aug 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include <string>



namespace sqloxx
{

class DatabaseConnection;

/**
 * Class for managing the storage and retrieval of objects to and 
 * from a database via a DatabaseConnection.
 *
 * This class should be specialized for T where you want objects of
 * type T to be managed in this way.
 *
 * None of the members of StorageManager are actually implemented, except
 * in full specializations of the class. The purpose of actually declaring
 * the members in the general template is really just one of documentation.
 *
 * @todo Implement this for all the phatbooks business object classes.
 * This will also enable a simplification of a lot of the code in
 * PhatbooksDatabaseConnection.
 * 
 * @todo Documentation, testing, everything...
 *
 * @todo In transitioning from the old PhatbooksDatabaseConnection
 * store methods, I stopped the checking implicit in the key lookups,
 * to make sure the next key won't cause overflow. I need to reimplement
 * that somehow.
 */
template <class T>
class StorageManager
{
public:
	typedef std::string Key;
	static void save(T const& obj, DatabaseConnection&);
	static T load(Key const& key, DatabaseConnection&);
	static void setup_tables();
};





}  // namespace sqloxx

#endif  // GUARD_storage_manager_hpp
