#ifndef GUARD_identity_map_hpp
#define GUARD_identity_map_hpp

#include "handle.hpp"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
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


template <typename T, typename Connection>
class IdentityMap
{
public:

	typedef typename T::Id Id;
	typedef typename T::Id ProxyKey;

	IdentityMap(Connection& p_connection);

	/**
	 * Provide handle to object of T, representing a newly created object
	 * that has not yet been persisted to the database
	 */
	Handle<T> provide_object();

	/**
	 * Provide handle to object of type T, representing an object
	 * already stored in the database, with id p_id.
	 */
	Handle<T> provide_object(Id p_id);
	
	/**
	 * Register id of newly saved T.
	 */
	void register_id(ProxyKey proxy_key, Id allocated_id);

	void erase_object_proxied(ProxyKey proxy_key);

	/**
	 * Notify the IdentityMap that there are no handles left that are
	 * pointing to this object.
	 */
	void notify_nil_handles(ProxyKey proxy_key);

	void enable_caching();

	void disable_caching();

	Connection& connection();

	IdentityMap& operator=(IdentityMap const& rhs);

private:

	// Find the next available proxy key
	// WARNING Move the implementation out of the class body.
	ProxyKey next_proxy_key();
	typedef typename boost::shared_ptr<T> Record;
	typedef boost::unordered_map<Id, Record> IdMap;
	typedef std::map<ProxyKey, Record> ProxyKeyMap;

	ProxyKeyMap& proxy_map() const
	{
		return m_map_data->proxy_map;
	}

	IdMap& id_map() const
	{
		return m_map_data->id_map;
	}

	bool& caching() const
	{
		return m_map_data->caching;
	}

	// Data members

	struct MapData
	{
		MapData(Connection& p_connection):
			connection(p_connection),
			caching(false)
		{
		}
		ProxyKeyMap proxy_map;  // For all objects.
		IdMap id_map;        // For objects that exist in the database.
		// Indicates whether the IdentityMap is currently
		// holding objects indefinitely in the cache (m_caching == true),
		// or whether it is
		// clearing each object out when there are no longer handles
		// pointing to it (m_caching == false).
		Connection& connection;     
		bool caching; 
	};
	boost::shared_ptr<MapData> m_map_data;
};


template <typename T, typename Connection>
IdentityMap<T, Connection>::IdentityMap(Connection& p_connection):
	m_map_data(new MapData(p_connection))
{
}

template <typename T, typename Connection>
Handle<T>
IdentityMap<T, Connection>::provide_object()
{
	
	Record obj_ptr((new T(*this)));
	ProxyKey const proxy_key = next_proxy_key();
	obj_ptr->set_proxy_key(proxy_key);
	proxy_map()[proxy_key] = obj_ptr;
	return Handle<T>(obj_ptr.get());
}


template <typename T, typename Connection>
Handle<T>
IdentityMap<T, Connection>::provide_object(Id p_id)
{
	typename IdMap::iterator it = id_map().find(p_id);
	if (it == id_map().end())
	{
		// Then we need to create this object.
		Record obj_ptr(new T(*this, p_id));
		id_map()[p_id] = obj_ptr;
		ProxyKey proxy_key = next_proxy_key();
		proxy_map()[proxy_key] = obj_ptr;
		obj_ptr->set_proxy_key(proxy_key);
		return Handle<T>(obj_ptr.get()); 
	}
	assert (it != id_map().end());
	return Handle<T>(it->second.get());
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::register_id(ProxyKey proxy_key, Id allocated_id)
{
	id_map()[allocated_id] = proxy_map()[proxy_key];
	return;
}


template <typename T, typename Connection>
void
IdentityMap<T, Connection>::erase_object_proxied(ProxyKey proxy_key)
{
	Record const record = proxy_map().find(proxy_key)->second;
	if (record->has_id())
	{
		assert (id_map().find(record->id()) != id_map().end());
		id_map().erase(record->id());
	}
	proxy_map().erase(proxy_key);
	return;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::notify_nil_handles(ProxyKey proxy_key)
{
	if (!caching())
	{
		erase_object_proxied(proxy_key);
	}
	return;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::enable_caching()
{
	caching() = true;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::disable_caching()
{
	if (caching())
	{
		typename ProxyKeyMap::iterator const endpoint = proxy_map().end();
		for
		(	typename ProxyKeyMap::iterator it = proxy_map().begin();
			it != endpoint;
			++it
		)
		{
			if (it->second->is_orphaned())
			{
				erase_object_proxied(it->first);
			}
		}
		caching() = false;
	}
	return;
}

template <typename T, typename Connection>
Connection&
IdentityMap<T, Connection>::connection()
{
	return m_map_data->connection;
}

template <typename T, typename Connection>
IdentityMap<T, Connection>&
IdentityMap<T, Connection>::operator=(IdentityMap const& rhs)
{
	m_map_data = rhs.m_map_data;
	return *this;
}

template <typename T, typename Connection>
typename IdentityMap<T, Connection>::ProxyKey
IdentityMap<T, Connection>::next_proxy_key()
{
	// TODO Change this so that vacated slots are filled, rather
	// than just always taking least - 1. Currently there is
	// a danger that we will just forever move towards min, until
	// we overflow.

	// Using negative numbers to avoid any possible confusion
	// with Id.
	// Relies on this being a std::map, in which the first
	// key is less than any other key.
	ProxyKey const least = proxy_map().begin()->first;
	if (least == std::numeric_limits<ProxyKey>::min())
	{
		throw OverflowException("Proxy key has reached numeric limit.");
	}
	return least - 1;
}

}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
