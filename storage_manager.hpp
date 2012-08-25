#ifndef GUARD_storage_manager_hpp
#define GUARD_storage_manager_hpp

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
	typedef std::string Key;
public:
	StorageManager(DatabaseConnection& p_database_connection);
	void save(T const& obj);
	T load(Key const& key);
	void setup_tables();
private:
	DatabaseConnection& m_database_connection;

};





}  // namespace sqloxx

#endif  // GUARD_storage_manager_hpp
