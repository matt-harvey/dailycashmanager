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
class IdentityMap: public boost::noncopyable
{
public:

	typedef typename T::Id Id;

	Handle<T> provide_object
	(	boost::shared_ptr<DatabaseConnection> dbc,
		Id p_id
	);

private:

	typedef typename boost::shared_ptr<T> Record;
	typedef boost::unordered_map<Id, Record> Map;
	Map m_map;
};

template <typename T>
Handle<T>
IdentityMap<T>::provide_object
(	boost::shared_ptr<DatabaseConnection> dbc,
	Id p_id
)
{
	typedef Handle<T> Handle;
	typename Map::iterator it = m_map.find(p_id);
	if (it == m_map.end())
	{
		// Then we need to create this object.
		Record obj_ptr(new T(dbc, p_id));
		m_map[p_id] = obj_ptr;
		return Handle(obj_ptr); 
	}
	assert (it != m_map.end());
	return Handle(it->second);
}
		

}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
