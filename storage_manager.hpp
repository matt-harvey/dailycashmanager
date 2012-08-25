#ifndef GUARD_storage_manager_hpp
#define GUARD_storage_manager_hpp

#include <boost/shared_ptr.hpp>
#include <string>

namespace sqloxx
{

template<typename T>
class StorageManager
{
	typedef std::string Key;
public:
	void save(T const& obj);
	T load(Key key);
	static void setup_table();
	static std::string table_name();
	static std::string key_name();
private:
	boost::shared_ptr<DatabaseConnection> m_database_connection;

};



}  // namespace sqloxx

#endif  // GUARD_storage_manager_hpp
