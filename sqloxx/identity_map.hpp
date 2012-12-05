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

#ifdef DEBUG
	#include <jewel/debug_log.hpp>
	#include <iostream>
	#include <typeinfo>
#endif

namespace sqloxx
{


// Forward declaration
template <typename Derived, typename Connection>
class PersistentObject;


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
 * @todo Testing.
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
	 * @returns a Handle<T> pointing to a newly constructed instance of T,
	 * that is cached in this instance of IdentityMap<T, Connection>.
	 *
	 * @throws sqloxx::OverflowException in the extremely unlikely
	 * event that the in-memory cache already has so many objects loaded that
	 * an additional object could not be cached without causing
	 * arithmetic overflow in the process of assigning it a key.
	 *
	 * @throws std::bad_alloc in the unlikely event of memory allocation
	 * failure during the creating and caching of the instance of T.
	 *
	 * <em>In addition</em>, any exceptions thrown from the T constructor
	 * of the form T(IdentityMap<T, Connection>&) may also be thrown
	 * from provide_object().
	 *
	 * Exception safety depends on the constructor of T of the form
	 * T(IdentityMap<T, Connection>&). Provided this constructor offers at
	 * least the <em>strong guarantee</em>, then provide_object() offers the
	 * <em>strong guarantee</em> (although there may be some internal cache
	 * state that is not rolled back but which does not affect client code).
	 *
	 * @todo Testing.
	 */
	Handle<T> provide_object();

	/**
	 * Provide handle to object of type T, representing an object
	 * already stored in the database, with primary key (id) p_id.
	 *
	 * @returns a Handle<T> pointing to an instance of T corresponding
	 * to a record of the corresponding type already persisted in the
	 * database, with p_id as its primary key.
	 *
	 * @throws std::bad_alloc if the object is not already loaded in the
	 * cache, and there is a memory allocation failure in the process of
	 * loading and caching the object.
	 *
	 * @throws sqloxx::OverflowException in the extremely unlikely
	 * event that the in-memory cache already has so many objects loaded that
	 * an additional object could not be cached without causing
	 * arithmetic overflow in the process of assigning it a key.
	 *
	 * <em>In addition</em>, any exceptions thrown from the T constructor
	 * of the form T(IdentityMap<T, Connection>&, typename T::Id) may
	 * also be thrown from provide_object().
	 *
	 * Exception safety depends on the constructor of T of the form
	 * T(IdentityMap<T, Connection>&, typename T::Id). Provided this
	 * constructor offers at
	 * least the <em>strong guarantee</em>, then provide_object() offers the
	 * <em>strong guarantee</em> (although there may be some internal cache
	 * state that is not rolled back but which does not affect client code).
	 * For this guarantee to hold, it is also required that the destructor
	 * of T not throw.
	 *
	 * @todo Testing.
	 */
	Handle<T> provide_object(Id p_id);

	/**
	 * Turn on caching. When caching is on, objects loaded from the
	 * database are cached indefinitely in the IdentityMap. When
	 * caching is off, each object is only cached as long as there
	 * as at least one Handle referring to it. If enable_caching() is
	 * called when caching is already on, it has no effect.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void enable_caching();

	/**
	 * Turn caching off. When caching is off, each object is only
	 * cached as long as there is at least one Handle referring to
	 * it. If disable_caching() is called when caching is already
	 * off, it has no effect. If caching is on when disable_caching()
	 * is called, then the function will remove from the cache any
	 * object that currently has no sqloxx::Handle<T> instances
	 * pointing to it.
	 * 
	 * Exception safety: <em>nothrow guarantee</em>, providing the
	 * destructor of T does not throw.
	 *
	 * @todo Testing.
	 */
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

	/**
	 * Control access to the various functions of the class
	 * IdentityMap<T, Connection>, deliberately
	 * limiting this access to the class PersistentObject<T, Connection>.
	 */
	class Attorney
	{
	public:

		friend class PersistentObject<T, Connection>;

	private:

		static void register_id
		(	IdentityMap& p_identity_map,
			CacheKey p_cache_key,
			Id p_id
		)
		{
			p_identity_map.register_id(p_cache_key, p_id);
			return;
		}

		static void deregister_id
		(	IdentityMap& p_identity_map,
			Id p_id
		)
		{
			p_identity_map.deregister_id(p_id);
			return;
		}

		static void notify_nil_handles
		(	IdentityMap& p_identity_map,
			CacheKey p_cache_key
		)
		{
			p_identity_map.notify_nil_handles(p_cache_key);
			return;
		}

		static void uncache_object
		(	IdentityMap& p_identity_map,	
			CacheKey p_cache_key
		)

		{
			p_identity_map.uncache_object(p_cache_key);
			return;
		}
	};

	friend class Attorney;

private:

	/**
	 * Register id of newly saved instance of T. This function is
	 * intended only to be called from PersistentObject<T, Connection>.
	 * This tells the cache the id of the object so that in future, it
	 * can be looked up by its id as well as by its cache key.
	 *
	 * Precondition: there must be an object cached in the IdentityMap
	 * with this p_cache_key.
	 *
	 * @param p_cache_key the cache key of the newly saved object
	 *
	 * @param p_id the id of the newly saved object, which corresponds
	 * to its primary key in the database
	 *
	 * @throws std::bad_alloc in the very unlikely event of memory allocation
	 * failure while registering the object's id in the cache.
	 *
	 * Exception safety: <em>strong guarantee</em>, providing the precondition
	 * is met.
	 *
	 * @todo Testing.
	 */
	void register_id(CacheKey p_cache_key, Id p_id);

	/**
	 * Notify IdentityMap that an instance of T that previously had
	 * an id p_id, no longer has any id. This function is
	 * intended only to be called from PersistentObject<T, Connection>.
	 *
	 * @param p_id the id of the instance of T, corresponding to its
	 * primary key in the database.
	 *
	 * Preconditions:\n
	 * It must be known of the instance of T in
	 * question, that it is cached in the IdentityMap under p_id
	 * (not as its cache key, but as its id); and\n
	 * The destructor of T must be non-throwing.
	 *
	 * Exception safety: <em>nothrow guarantee</em>, providing the
	 * preconditions are met.
	 */
	void deregister_id(Id p_id);

	/**
	 * This should only be called from PersistentObject<T, Connection>.
	 *
	 * Notify the IdentityMap that there are no handles left that are
	 * pointing to the object with p_cache_key.
	 * 
	 * Preconditions: (a) there must be an object cached in this
	 * IdentityMap with this cache_key; and (b) the destructor of T must
	 * never throw.
	 * 
	 * Exception safety: <em>nothrow guarantee</em>, provided the
	 * preconditions are met.
	 *
	 * @todo Testing.
	 */
	void notify_nil_handles(CacheKey p_cache_key);

	/**
	 * This should only be called by PersistentObject<T, Connection>.
	 *
	 * Preconditions:\n
	 * The destructor of T must be nothrow; and\n
	 * There is an object cached under p_cache_key in the cache_key_map.
	 *
	 * Exception safety: <em>nothrow guarantee</em>, provided
	 * preconditions are met.
	 */
	void uncache_object(CacheKey p_cache_key);


	// Find the next available cache key
	// WARNING Move the implementation out of the class body.
	/**
	 * @throws sqloxx::OverflowException if the cache has reached
	 * its maximum size (extremely unlikely).
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
	 * Exception safety: <nothrow guarantee</em>
	 */
	IdMap& id_map() const
	{
		return m_map_data->id_map;
	}

	CacheKey& last_cache_key() const
	{
		return m_map_data->last_cache_key;
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
			last_cache_key(0),
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

		// The last key to be assigned as in index into cache_key_map - or
		// 0 if none have been assigned.
		CacheKey last_cache_key;

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
	// Comments here are to help ascertain exception-safety.
	Record obj_ptr(new T(*this));  // T-dependant exception safety
	CacheKey const cache_key = provide_cache_key(); // strong guarantee

	// In the next statement:
	// constructing the pair of CacheKeyMap::value_type is nothrow; and
	// calling insert either (a) succeeds, or (b) fails completely and
	// throws std::bad_alloc. If it throws, then obj_ptr
	// will be deleted on exit (as it's a shared_ptr) - which amounts to
	// rollback of provide_object().
	cache_key_map().insert
	(	typename CacheKeyMap::value_type(cache_key, obj_ptr)
	);
	// We could have done the following, but the above is more efficient and
	// less "magical".
	// cache_key_map()[cache_key] = obj_ptr; 

	// Nothrow
	PersistentObject<T, Connection>::CacheKeyAttorney::set_cache_key
	(	*obj_ptr,
		cache_key
	);	

	// In the below, get() is nothrow. The Handle<T> constructor and copy
	// constructor can throw in some (very unlikely) circumstances,
	// namely when there are too many Handle<T> instances pointing to
	// this T; but that's not the case here, as we have only just
	// constructed this object and are returning the only Handle so
	// far pointing to it. So returning the return value is nothrow.
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

		// Exception safety here depends on T.
		Record obj_ptr(new T(*this, p_id));

		// atomic, possible sqloxx::OverflowException
		CacheKey const cache_key = provide_cache_key();

		// atomic, possible std::bad_alloc
		id_map().insert(typename IdMap::value_type(p_id, obj_ptr));
		try
		{
			cache_key_map().insert
			(	typename IdMap::value_type(cache_key, obj_ptr)
			);
		}
		catch (std::bad_alloc&)
		{
			id_map().erase(p_id);
			throw;
		}

		// Nothrow
		PersistentObject<T, Connection>::CacheKeyAttorney::set_cache_key
		(	*obj_ptr,
			cache_key
		);

		// We know this won't throw sqloxx::OverflowError, as it's a
		// newly loaded object.
		return Handle<T>(obj_ptr.get()); 
	}
	assert (it != id_map().end());
	if
	(	PersistentObject
		<	T,
			Connection
		>::HandleMonitorAttorney::has_high_handle_count(*(it->second))
	)
	{
		throw sqloxx::OverflowException
		(	"Handle count for has reached dangerous level. "
		);
	}
	return Handle<T>(it->second.get());
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::register_id(CacheKey p_cache_key, Id p_id)
{
	typename CacheKeyMap::const_iterator const finder =
		cache_key_map().find(p_cache_key);
	
	// todo This should be a require not an assert
	assert (finder != cache_key_map().end());

	id_map().insert(typename IdMap::value_type(p_id, finder->second));
	return;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::deregister_id(Id p_id)
{
	// todo This should be a require not an assert
	assert (id_map().find(p_id) != id_map().end());
	
	id_map().erase(p_id);
	return;
}


template <typename T, typename Connection>
void
IdentityMap<T, Connection>::uncache_object(CacheKey p_cache_key)
{
	// todo This should be a require not an assert
	assert (cache_key_map().find(p_cache_key) != cache_key_map().end());
	Record const record = cache_key_map().find(p_cache_key)->second;
	if (record->has_id())
	{
		assert (id_map().find(record->id()) != id_map().end());
		id_map().erase(record->id());
	}
	cache_key_map().erase(p_cache_key);
	return;
}

template <typename T, typename Connection>
void
IdentityMap<T, Connection>::notify_nil_handles(CacheKey p_cache_key)
{
	// todo This should be a require not an assert	
	assert (cache_key_map().find(p_cache_key) != cache_key_map().end());
	if (!is_caching())
	{
		uncache_object(p_cache_key);
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
			if
			(	PersistentObject
				<	T,
					Connection
				>::HandleMonitorAttorney::is_orphaned(*(it->second))
			)
			{
				uncache_object(it->first);
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
	typename CacheKeyMap::size_type const sz = cache_key_map().size();
	if (sz == 0)
	{
		return last_cache_key() = 1;  // Intentional assignment
	}
	if (sz == boost::numeric_cast<typename CacheKeyMap::size_type>(maximum))
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
	CacheKey ret = last_cache_key();
	typedef typename CacheKeyMap::const_iterator Iterator;
	Iterator it = cache_key_map().find(ret);
	Iterator const endpoint = cache_key_map().end();
	if (it == endpoint)
	{
		return last_cache_key();
	}

	// Look for the first available unused key to assign to next_cache_key()
	// ready for the next call to provide_cache_key(). This relies on
	// CacheKeyMap being, or behaving like, std::map, in that it keeps its
	// elements ordered by key.
	assert (cache_key_map().size() > 0);
	while (ret == it->first)
	{
		if (ret == maximum) ret = 1;
		else ++ret;
		if (++it == endpoint) it = cache_key_map().begin();
	}
	assert (cache_key_map().find(ret) == cache_key_map().end());
	return last_cache_key() = ret;  // Intentional assignment
}



/*
template <typename T, typename Connection>
inline
void
IdentityMap<T, Connection>::Attorney::register_id
(	IdentityMap& p_identity_map,
	IdentityMap::CacheKey p_cache_key,
	IdentityMap::Id p_id
)
{
	p_identity_map.register_id(p_cache_key, p_id);
	return;
}


template <typename T, typename Connection>
inline
void
IdentityMap<T, Connection>::Attorney::deregister_id
(	IdentityMap& p_identity_map,
	IdentityMap::Id p_id
)
{
	p_identity_map.deregister_id(p_id);
	return;
}


template <typename T, typename Connection>
inline
void
IdentityMap<T, Connection>::Attorney::notify_nil_handles
(	IdentityMap& p_identity_map,
	IdentityMap::CacheKey p_cache_key
)
{
	p_identity_map.notify_nil_handles(p_cache_key);
	return;
}


template <typename T, typename Connection>
inline
void
IdentityMap<T, Connection>::Attorney::uncache_object
(	IdentityMap& p_identity_map,
	IdentityMap::CacheKey p_cache_key
)
{
	p_identity_map.uncache_object(p_cache_key);
	return;
}
*/


}  // namespace sqloxx

#endif  // GUARD_identity_map_hpp
