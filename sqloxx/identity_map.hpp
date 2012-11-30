#ifndef GUARD_identity_map_hpp
#define GUARD_identity_map_hpp

#include "handle.hpp"
#include <boost/noncopyable.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <cassert>
#include <map>
#include <stdexcept>

#ifndef DEBUG
	#include <jewel/debug_log.hpp>
	#include <iostream>
	#include <typeinfo>
#endif

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
 * pointers to these objects. By using IdentityMap to cache objects,
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
	typedef typename T::Id CacheKey;

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
	 * by the new IdentityMap will be the very same as those employed
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
	 * Assignment is shallow, and with semantics like those of the
	 * copy constructor.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 *
	 * @todo Testing.
	 */
	IdentityMap& operator=(IdentityMap const& rhs);

	/**
	 * Provide handle to object of type T, representing a newly created object
	 * that has not yet been persisted to the database.
	 *
	 */
	Handle<T> provide_object();

	/**
	 * Provide handle to object of type T, representing an object
	 * already stored in the database, with primary key (id) p_id.
	 */
	Handle<T> provide_object(Id p_id);
	
	/**
	 * Register id of newly saved instance of T. This function is
	 * intended only to be called from PersistentObject<T, Connection>.
	 */
	void register_id(CacheKey cache_key, Id allocated_id);

	/**
	 * Notify the IdentityMap that there are no handles left that are
	 * pointing to this object.
	 */
	void notify_nil_handles(CacheKey cache_key);

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

	void uncache_object_proxied(CacheKey cache_key);

	// Find the next available cache key
	// WARNING Move the implementation out of the class body.
	/**
	 * @throws sqloxx::OverflowException if the cache has reached
	 * its maximum size (extremely unlikely). In this event, state
	 * is unaltered from pre-call.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	CacheKey provide_cache_key();

	typedef typename boost::shared_ptr<T> Record;
	typedef boost::unordered_map<Id, Record> IdMap;
	typedef std::map<CacheKey, Record> CacheKeyMap;

	/**
	 * Returns a reference to the underlying cache in which objects
	 * are indexed by CacheKey (as opposed to Id, which does not apply
	 * to objects not yet persisted to the database.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	CacheKeyMap& cache_key_map() const
	{
		return m_map_data->cache_key_map;
	}

	/**
	 * Returns a reference to the underlying cache in which objectsa
	 * are indexed Id (ass opposed to CacheKey.
	 *
	 * ExceptioSafety: <nothrow guarantee</em>
	 */
	IdMap& id_map() const
	{
		return m_map_data->id_map;
	}

	CacheKey& next_cache_key() const
	{
		return m_map_data->next_cache_key;
	}

	bool& is_caching() const
	{
		return m_map_data->is_caching;
	}

	// Data members

	// Hold data in a pimpl struct merely to facilitate safe, shallow copying.
	struct MapData
	{
		MapData(Connection& p_connection):
			connection(p_connection),
			next_cache_key(1),
			is_caching(false)
		{
		}
		
		// Provides index to all cached objects, including those not as yet
		// saved to the database.
		CacheKeyMap cache_key_map;

		// Provides index to objects that have been persisted to the database,
		// indexed by their primary key in the database.
		IdMap id_map;

		// The database connection with which this IdentityMap is associated.
		Connection& connection; 

		// The next key to be assigned as in index into cache_key_map.
		CacheKey next_cache_key;

		// Indicates whether the IdentityMap is currently
		// holding objects indefinitely in the cache (m_caching == true),
		// or whether it is
		// clearing each object out when there are no longer handles
		// pointing to it (m_caching == false).
		bool is_caching; 
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
	Record obj_ptr(new T(*this));
	CacheKey const cache_key = provide_cache_key();
	obj_ptr->set_cache_key(cache_key);
	cache_key_map()[cache_key] = obj_ptr;
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
		CacheKey cache_key = provide_cache_key();
		cache_key_map()[cache_key] = obj_ptr;
		obj_ptr->set_cache_key(cache_key);
		return Handle<T>(obj_ptr.get()); 
	}
	assert (it != id_map().end());
	return Handle<T>(it->second.get());
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::register_id(CacheKey cache_key, Id allocated_id)
{
	id_map()[allocated_id] = cache_key_map()[cache_key];
	return;
}


template <typename T, typename Connection>
void
IdentityMap<T, Connection>::uncache_object_proxied(CacheKey cache_key)
{
	Record const record = cache_key_map().find(cache_key)->second;
	if (record->has_id())
	{
		assert (id_map().find(record->id()) != id_map().end());
		id_map().erase(record->id());
	}
	cache_key_map().erase(cache_key);
	return;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::notify_nil_handles(CacheKey cache_key)
{
	if (!is_caching())
	{
		uncache_object_proxied(cache_key);
	}
	return;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::enable_caching()
{
	is_caching() = true;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::disable_caching()
{
	if (is_caching())
	{
		typename CacheKeyMap::iterator const endpoint = cache_key_map().end();
		for
		(	typename CacheKeyMap::iterator it = cache_key_map().begin();
			it != endpoint;
			++it
		)
		{
			if (it->second->is_orphaned())
			{
				uncache_object_proxied(it->first);
			}
		}
		is_caching() = false;
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
typename IdentityMap<T, Connection>::CacheKey
IdentityMap<T, Connection>::provide_cache_key()
{
	static CacheKey const maximum = std::numeric_limits<CacheKey>::max();
	if
	(	cache_key_map().size() ==
		boost::numeric_cast<typename CacheKeyMap::size_type>(maximum)
	)
	{
		// There are no more available positive numbers to serve
		// as cache keys. This is extremely unlikely ever to occur.
		// We could possibly avoid throwing here by instead calling
		// disable_caching(), which would trigger an emptying of the cache
		// of any orphaned objects. But the emptying might take a long, long
		// time. So we just throw.
		// Avoid complication by not even considering negative numbers.
		throw OverflowException
		(	"No more cache keys are available for identifying objects "
			"in the IdentityMap."
		);
	}
	CacheKey const ret = next_cache_key();
	typedef typename CacheKeyMap::const_iterator Iterator;
	Iterator it = cache_key_map().find(ret);
	Iterator const endpoint = cache_key_map().end();
	CacheKey current_key = ret;

	// Look for the first available unused key to assign to next_cache_key()
	// ready for the next call to provide_cache_key(). This relies on
	// CacheKeyMap being, or behaving like, std::map, in that it keeps its
	// elements ordered by key.
	while (current_key == it->first)
	{
		if (current_key == maximum) current_key = 0;
		else ++current_key;
		if (++it == endpoint) it = cache_key_map().begin();
	}
	assert (cache_key_map().find(current_key) == cache_key_map().end());
	next_cache_key() = current_key;
	return ret;
}

}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
