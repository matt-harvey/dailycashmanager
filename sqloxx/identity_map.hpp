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
	 * Provide handle to object of T, representing a newly created object
	 * that has not yet been persisted to the database
	 *
	 * Connection is the subclass of DatabaseConnection representing a
	 * database connection of the client application.
	 */
	template <typename Connection>
	Handle<T> provide_object(boost::shared_ptr<Connection> dbc);

	/**
	 * Provide handle to object of type T, representing an object
	 * already stored in the database, with id p_id.
	 *
	 * Connection is the subclass of DatabaseConnection representing
	 * a database connection for the client application.
	 */
	template <typename Connection>
	Handle<T> provide_object
	(	boost::shared_ptr<Connection> dbc,
		Id p_id
	);
	
	/**
	 * Register id of newly saved T.
	 *
	 * WARNING Need proper implementation.
	 */
	void register_id(typename T::Id proxy_key, typename T::Id allocated_id)
	{
	}

private:

	typedef typename boost::shared_ptr<T> Record;
	typedef boost::unordered_map<Id, Record> Map;
	Map m_map;        // For storing objects that exist in the database.
	Map m_proxy_map;  // For storing newly created objects with no id.
};


template <typename T>
template <typename Connection>
Handle<T>
IdentityMap<T>::provide_object(boost::shared_ptr<Connection> dbc)
{
	Id i = -1;
	while (m_proxy_map.find(i) != m_proxy_map.end())
	{
		--i;
	}
	Record obj_ptr((new T(dbc)));
	obj_ptr->set_proxy_key(i);
	m_proxy_map[i] = obj_ptr;
	return Handle<T>(obj_ptr);
}




template <typename T>
template <typename Connection>
Handle<T>
IdentityMap<T>::provide_object
(	boost::shared_ptr<Connection> dbc,
	Id p_id
)
{
	typename Map::iterator it = m_map.find(p_id);
	if (it == m_map.end())
	{
		// Then we need to create this object.
		Record obj_ptr(new T(dbc, p_id));
		m_map[p_id] = obj_ptr;
		return Handle<T>(obj_ptr); 
	}
	assert (it != m_map.end());
	return Handle<T>(it->second);
}
		

}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
