#ifndef GUARD_identity_map_hpp
#define GUARD_identity_map_hpp

#include "handle.hpp"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <map>
#include <stdexcept>

// For debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	#include <typeinfo>
	using std::endl;

namespace sqloxx
{


template <typename T>
class IdentityMap: public boost::noncopyable
{
public:

	typedef typename T::Id Id;
	typedef Id ProxyKey;

	/**
	 * Provide handle to object of T, representing a newly created object
	 * that has not yet been persisted to the database
	 *
	 * Connection is the subclass of DatabaseConnection representing a
	 * database connection of the client application.
	 */
	template <typename Connection>
	Handle<T> provide_object(boost::shared_ptr<Connection> const& dbc);

	/**
	 * Provide handle to object of type T, representing an object
	 * already stored in the database, with id p_id.
	 *
	 * Connection is the subclass of DatabaseConnection representing
	 * a database connection for the client application.
	 */
	template <typename Connection>
	Handle<T> provide_object
	(	boost::shared_ptr<Connection> const& dbc,
		Id p_id
	);
	
	/**
	 * Register id of newly saved T.
	 *
	 * WARNING Move implementation out of class body.
	 */
	void register_id(ProxyKey proxy_key, Id allocated_id)
	{
		m_id_map[allocated_id] = m_proxy_map[proxy_key];
		/*
		JEWEL_DEBUG_LOG << "Id registered for id = " << allocated_id
		                << " with object type = "
						<< typeid(m_id_map[allocated_id]).name() << endl;
		*/
		return;
	}

	void erase_object_proxied(ProxyKey proxy_key)
	{
		Record const record = m_proxy_map.find(proxy_key)->second;
		if (record->has_id())
		{
			assert (m_id_map.find(record->id()) != m_id_map.end());
			m_id_map.erase(record->id());
		}
		m_proxy_map.erase(proxy_key);
		return;
	}


	/**
	 * Notify the IdentityMap that there are no handles left that are
	 * pointing to this object.
	 *
	 * WARNING Move implementation out of class body. 
	 */
	void notify_nil_handles(ProxyKey proxy_key)
	{
		if (!m_caching)
		{
			erase_object_proxied(proxy_key);
		}
		return;
	}


	void enable_caching()
	{
		m_caching = true;
	}

	void disable_caching();


private:

	// Find the next available proxy key
	// WARNING Move the implementation out of the class body.
	ProxyKey next_proxy_key();
	typedef typename boost::shared_ptr<T> Record;
	typedef boost::unordered_map<Id, Record> IdMap;
	typedef std::map<ProxyKey, Record> ProxyKeyMap;
	ProxyKeyMap m_proxy_map;  // For all objects.
	IdMap m_id_map;        // For objects that exist in the database.
	// Indicates whether the IdentityMap is currently
	// holding objects indefinitely in the cache (m_caching == true),
	// or whether it is
	// clearing each object out when there are no longer handles
	// pointing to it (m_caching == false).
	bool m_caching; 
	         
};


template <typename T>
template <typename Connection>
Handle<T>
IdentityMap<T>::provide_object(boost::shared_ptr<Connection> const& dbc)
{
	
	Record obj_ptr((new T(dbc)));
	ProxyKey const proxy_key = next_proxy_key();
	obj_ptr->set_proxy_key(proxy_key);
	m_proxy_map[proxy_key] = obj_ptr;
	return Handle<T>(obj_ptr);
}


template <typename T>
template <typename Connection>
Handle<T>
IdentityMap<T>::provide_object
(	boost::shared_ptr<Connection> const& dbc,
	Id p_id
)
{
	typename IdMap::iterator it = m_id_map.find(p_id);
	if (it == m_id_map.end())
	{
		// Then we need to create this object.
		Record obj_ptr(new T(dbc, p_id));
		m_id_map[p_id] = obj_ptr;
		ProxyKey proxy_key = next_proxy_key();
		m_proxy_map[proxy_key] = obj_ptr;
		obj_ptr->set_proxy_key(proxy_key);
		return Handle<T>(obj_ptr); 
	}
	assert (it != m_id_map.end());
	return Handle<T>(it->second);
}

template <typename T>
void
IdentityMap<T>::disable_caching()
{
	if (m_caching)
	{
		for
		(	typename ProxyKeyMap::iterator it = m_proxy_map.begin();
			it != m_proxy_map.end();
			++it
		)
		{
			if (it->second->is_orphaned())
			{
				erase_object_proxied(it->first);
			}
		}
		m_caching = false;
	}
	return;
}
	
template <typename T>
typename IdentityMap<T>::ProxyKey
IdentityMap<T>::next_proxy_key()
{
	// TODO Change this so that vacated slots are filled, rather
	// than just always taking least - 1. Currently there is
	// a danger that we will just forever move towards min, until
	// we overflow.

	// Using negative numbers to avoid any possible confusion
	// with Id.
	// Relies on this being a std::map, in which the first
	// key is less than any other key.
	ProxyKey const least = m_proxy_map.begin()->first;
	if (least == std::numeric_limits<ProxyKey>::min())
	{
		throw OverflowException("Proxy key has reached numeric limit.");
	}
	return least - 1;
}

	

}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
