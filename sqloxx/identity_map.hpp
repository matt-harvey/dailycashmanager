#ifndef GUARD_identity_map_hpp
#define GUARD_identity_map_hpp

#include "handle.hpp"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace sqloxx
{

class DatabaseConnection;  // forward declaration

// When PersistentObject::save_new() is called, it needs
// to let the IdentityManager know.

template <typename T>
class IdentityManager: public boost::noncopyable
{
public:

	typedef T::Id Id;

	Handle<T> provide_object
	(	boost::shared_ptr<DatabaseConnection> dbc,
		Id p_id
	);

private:

	struct Record
	{
		Record():
			object_pointer(0),
			live_count(0)
		{
		}

		explicit Record(boost::shared_ptr<PersistentObject> ptr):
			object_pointer(ptr),
			live_count(0)
		{
		}

		boost::shared_ptr<PersistentObject> object_pointer;

		int live_count;  // number of handles to this object "at large"
	};

	typedef boost::unordered_map<Id, Record> Map;
	Map m_map;
};

template <typename T>
Handle<T>
provide_object(boost::shared_ptr<DatabaseConnection> dbc, T::Id p_id)
{
	Map::iterator it = m_map.find(p_id);
	if (it == record_map.end())
	{
		// Then we need to create this object.
		boost::shared_ptr<T> obj_ptr(new T(dbc, p_id));
		record_map[p_id] = Record(obj_ptr);
		return Handle<T>(obj_ptr); 
	}
	assert (it != record_map.end());
	return Handle(it->second.object_pointer);
}
		

}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
