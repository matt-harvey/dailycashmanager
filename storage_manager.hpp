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
	T load(Key key);
	static void setup_table();
	static std::string table_name();
	static std::string key_name();
private:
	DatabaseConnection& m_database_connection;

};





}  // namespace sqloxx

#endif  // GUARD_storage_manager_hpp
