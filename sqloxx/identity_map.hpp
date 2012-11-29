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


/**
 * Provides an in-memory cache for objects of type T, where such
 * objects are persisted to a database via a database connection of
 * type Connection. T and Connection are passed as template parameters
 * to the class template. It is expected that T is a subclass of
 * sqloxx::PersistentObject<T, Connection>, and Connection is a
 * subclass of sqloxx::DatabaseConnection.
 *
 * Each instance of IdentityMap has a particular Connection associated
 * with it. The IdentityMap caches objects loaded from the database,
 * and provides clients - in particular the sqloxx::Handle<T> class -
 * pointers to these objects. My using IdentityMap to cache objects,
 * application code can be sure that each single record of type T
 * that is stored in the database, has at most a single in-memory
 * object of type T associated with that record, loaded in memory
 * at any one time. IdentityMap thus implements the "Identity Map"
 * pattern detailed in Martin Fowler's book, "Patterns of Enterprise
 * Application Architecture". By having at most a single in-memory
 * object per in-database record, we guard against the possibility
 * of the same object being edited inconsistently in different
 * locations. By keeping objects in a cache, we speed execution of
 * read and write operations, by avoiding a trip to the disk when an
 * object has already been loaded.
 *
 * IdentityMap is intended to work in conjunction with sqloxx::Handle<T>
 * and sqloxx::PersistentObject<T, Connection>. See also the documentation
 * for those classes.
 *
 * @todo Documentation and testing.
 */
template <typename T, typename Connection>
class IdentityMap
{
public:

	typedef typename T::Id Id;
	typedef typename T::Id ProxyKey;

	/**
	 * Construct an IdentityMap associated with the database
	 * connection Connection. Connection should be a subclass
	 * of sqloxx::DatabaseConnection.
	 *
	 * @throws std::bad_alloc in the case of memory allocation
	 * failure. (This is very unlikely.)
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *
	 * @todo Testing.
	 */
	IdentityMap(Connection& p_connection);

	/**
	 * Copy constructor. Performs a shallow copy. The underlying
	 * structures (e.g. database connection and cache) employed
	 * by the new IdentitMap will be the very same as those employed
	 * by rhs.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	IdentityMap(IdentityMap const& rhs);
	
	/**
	 * Destructor. The underlying cache is automatically emptied
	 * of all objects (i.e. instances of T) on destruction of
	 * the IdentityMap, by the calling the destructor of each
	 * object in the cache. The cache is then itself destructed.
	 * However the database connection (Connection instance) referenced
	 * by the IdentityMap is \e not destructed merely by virtue
	 * of the destruction of the IdentityMap.
	 *
	 * Exception safety: the <em>nothrow guarantee</em> is provided,
	 * providing the destructor of T does not throw.
	 *
	 * @todo Testing.
	 */
	~IdentityMap();

	/**
	 * Assignment is shallow, and behaves as per the copy constructor
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 *
	 * @todo Testing.
	 */
	IdentityMap& operator=(IdentityMap const& rhs);

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

	/**
	 * @returns a reference to the database connection with which
	 * this IdentityMap is associated.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 *
	 * @todo Testing.
	 */
	Connection& connection();

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

	// Hold in a struct to facilitate safe, shallow copying.
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
inline
IdentityMap<T, Connection>::IdentityMap(Connection& p_connection):
	m_map_data(new MapData(p_connection))
{
}

template <typename T, typename Connection>
inline
IdentityMap<T, Connection>::IdentityMap(IdentityMap const& rhs):
	m_map_data(rhs.m_map_data)
{
}

template <typename T, typename Connection>
inline
IdentityMap<T, Connection>::~IdentityMap()
{
}

template <typename T, typename Connection>
inline
IdentityMap<T, Connection>&
IdentityMap<T, Connection>::operator=(IdentityMap const& rhs)
{
	m_map_data = rhs.m_map_data;
	return *this;
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
inline
Connection&
IdentityMap<T, Connection>::connection()
{
	return m_map_data->connection;
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
