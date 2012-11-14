#ifndef GUARD_identity_manager_hpp
#define GUARD_identity_manager_hpp

#include "handle.hpp"
#include "persistent_object_handle.hpp"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace sqloxx
{

class DatabaseConnection;  // forward declaration


// Each PersistentObject would need to contain a pointer to
// the IdentityManager, or else to the DatabaseConnection, in which case
// then the DatabaseConnection would need to contain a pointer to
// the IdentityManager. When save_new is called on the PersistentObject, it
// then needs to let the IdentityManager know. Each PersistentObject already
// does have a pointer to the DatabaseConnection, so this could be used
// to obtain a reference to the IdentityManager.

class IdentityManager: public boost::noncopyable
{
public:

	typedef PersistentObject::Id Id;

	IdentityManager
	(	boost::shared_ptr<DatabaseConnection> p_database_connection,
		std::vector<int> const& record_map_identifiers
	);
	
	template <typename T>
	Handle<T>
	provide_persistent_object(Id p_id);

private:

	struct Record
	{
		Record(boost::shared_ptr<PersistentObject> ptr):
			object_pointer(ptr),
			live_count(0)
		{
		}
		boost::shared_ptr<PersistentObject> object_pointer;
		int live_count;  // number of handles to this object "at large"
	};
	typedef boost::unordered_map<Id, Record > RecordMap;
	typedef boost::shared_ptr<RecordMap> RecordMapPointer;
	typedef boost::unordered_map<int, RecordMapPointer> MasterMap;
	
	boost::shared_ptr<DatabaseConnection> m_database_connection;
	MasterMap m_master_map;

};

template <typename T>
Handle<T>
provide_persistent_object(Id p_id)
{
	MasterMap::iterator mmit = m_master_map.find(T::map_number());
	if (mmit == m_master_map.end())
	{
		throw std::runtime_error("No RecordMap with this map number.");
	}
	assert (mmit != m_master_map.end());
	RecordMap& record_map = *(it->second);
	RecordMap::iterator rmit = record_map.find(p_id);
	if (rmit == record_map.end())
	{
		// Then we need to create this object.
		boost::shared_ptr<T> obj_ptr(new T(m_database_connection, p_id));
		record_map[p_id] = obj_ptr;
		return Handle<T>(obj_ptr); 
	}
	assert (rmit != record_map.end());
	return rmit->second;
}
		

}  // namespace sqloxx

#endif  // GUARD_identity_manager_hpp
