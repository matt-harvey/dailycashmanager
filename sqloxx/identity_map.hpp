#ifndef GUARD_identity_map_hpp
#define GUARD_identity_map_hpp

#include "database_connection.hpp"
#include "handle.hpp"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace sqloxx
{


// When PersistentObject::save_new() is called, it needs
// to let the IdentityManager know.

template <typename T>
class IdentityMap: public boost::noncopyable
{
public:

	typedef typename T::Id Id;

	/**
	 * Connection should be DatabaseConnection or a subclass of
	 * DatabaseConnection.
	 */
	template <typename Connection>
	Handle<T> provide_object
	(	boost::shared_ptr<Connection> dbc,
		Id p_id
	);

	/**
	 * Register id of newly saved T.
	 */
	void register_id(typename T::Id proxy_key, typename T::Id allocated_id)
	{
		// WARNING Need proper implementation.
	}

private:

	typedef typename boost::shared_ptr<T> Record;
	typedef boost::unordered_map<Id, Record> Map;
	Map m_map;
};

template <typename T>
template <typename Connection>
Handle<T>
IdentityMap<T>::provide_object
(	boost::shared_ptr<Connection> dbc,
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
