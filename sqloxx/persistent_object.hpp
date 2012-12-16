#ifndef GUARD_persistent_object_hpp
#define GUARD_persistent_object_hpp

#include "database_transaction.hpp"
#include "general_typedefs.hpp"
#include "identity_map.hpp"
#include "next_auto_key.hpp"
#include "sql_statement.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/none.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <jewel/optional.hpp>
#include <exception>
#include <string>

#ifdef DEBUG
	#include <jewel/debug_log.hpp>
	#include <iostream>
#endif

namespace sqloxx
{

// Forward declaration
template <typename T>
class Handle;

template <typename T>
class PersistentObjectHandleAttorney;


/**
 * Class template for creating objects persisted to a database. This
 * should be inherited by a derived class that defines certain
 * functions as detailed below.
 *
 * An instance of (a class deriving from an instantiation of)
 * PersistentObject represents a "business object" for which the
 * data will be stored in a relational database (currently only
 * SQLite is supported) under a single primary key, that shall be
 * an auto-incrementing integer primary key.
 *
 * <b>IMPORTANT</b>: Only handle a PersistentObject instances via Handle
 * instances;\n
 * and always obtain Handle instances either: by copying or assigning existing
 * Handle instances; by calling
 * IdentityMap<Derived, Connection>::provide_handle(...); or
 * by calling get_handle<Derived>(...) (which in turn calls
 * IdentityMap<...>::provide_handle(...).
 *
 * <b>Identity Map Pattern</b>
 *
 * PersistentObject is intended
 * to be used in conjunction with the Identity Map pattern (as
 * detailed in Martin Fowler's book "Patterns of Enterprise
 * Architecture"). To enable this, sqloxx::PersistentObject is intended to
 * work in conjunction with sqloxx::IdentityMap and sqloxx::Handle.
 *
 * Say we have a client class derived from PersistentObject. Call this
 * class Derived. To obtain an instance of Derived, we call the
 * free-standing function sqloxx::get_handle<Derived>, declared in
 * database_connection.hpp. (See documentation therein for more details.)
 * 
  * This will return a Handle<Derived> to the underlying Derived
 * instance, the instance itself being cached in the IdentityMap<Derived>
 * that is associated with the DatabaseConnection passed to get_handle.
 * Instances of Derived should only ever be handled via a Handle<Derived>.
 * Handles can be copied around, dereferenced, and otherwise treated
 * similarly to a shared_ptr. The PersistentObject part of the Derived
 * instance, together
 * with Handle and IdentityMap, will work behind the scenes to ensure
 * that, for each record in the database, at most one object is loaded
 * into memory (i.e. cached in the IdentityMap). This prevents problems
 * with objects being edited across multiple instances.
 *	
 * There is nothing to prevent client code from using a further
 * wrapper class around Handle<Derived>. This then becomes a
 * special case of the "pimpl" pattern.
 *
 * See sqloxx::IdentityMap and sqloxx::Handle for further documentation
 * here.
 *
 *
 * <b>Lazy Loading</b>
 *
 * PersistentObject provides for lazy loading behaviour,
 * using the "ghost" pattern as described on p. 202 of Martin Fowler's
 * "Patterns of Enterprise Application Architecture". The PersistentObject
 * base class provides the bookkeeping associated with this pattern,
 * keeping track of the loading status of each in-memory object
 * ("loaded", "loading" or "ghost").
 *
 * in the Derived class, the intention is that some or all data members
 * declared in that class, can be "lazy". This means that they are not
 * initialized in the derived object's constructor, but are rather only
 * initialized at a later time via a call to load(), which in turn calls
 * the virtual method do_load() (which needs to be defined in the
 * derived class).
 *
 * In the derived class, implementations of getters
 * for attributes
 * other than those that are loaded immediately on construction, should
 * have \e load() as their first statement. (This means that getters in
 * Derived cannot
 * be const.) (See documentation for \e load().)
 *
 * In addition, implementations of \e all setters in the
 * derived class should have \e load() as their first statement.
 * Failure to adhere to these requirements will result in
 * in undefined behaviour.
 *
 * It is advisable to store lazy attributes in a boost::optional<T>, which
 * will result in loud, rather than silent, failure, in the event of an
 * attempt to access such an attribute before it has been initialized.
 *
 * Derived classes are free to initialize all attributes on construction of
 * an instance. This avoids the complications described above associated
 * with lazy loading, while giving up the potential runtime efficiencies
 * that lazy loading can provide.
 * 
 *
 * <b>Virtual functions</b>
 *
 * The following functions need to be provided with definitions
 * provided in the Derived class:
 *
 * <em>static std::string primary_table_name();</em>\n
 * Should return name of table in which instances of the derived class
 * are persisted in the database. If instance are persisted across
 * multiple tables, this function should return the "primary table",
 * i.e. a table containing the primary key for this class, such that
 * every persisted instance of this class has a row in this table.
 *
 * <em>virtual void do_load() = 0;</em>\n
 * See documentation of load() function.
 *
 * <em>virtual void do_save_existing() = 0;</em>\n
 * See documentation for save_existing() function.
 *
 * <em>virtual void do_save_new() = 0;</em>\n
 * See documentation for save_new() function.
 *
 * <em>virtual void do_ghostify() = 0;\n
 * See documentation for ghostify() function.
 *
 * In addition the following function \e may be provided with a definition
 * in the Derived class, although the PersistentObject base class provides
 * a default implementation which is suitable in many cases:
 *
 * <em>virtual void do_remove();\n
 * See documentation for remove() function.
 *
 *
 * <b>Derived class static functions</b>
 *
 * The folliwng functions need to be defined by the Derived class:
 *
 * static std::string Derived::primary_table_name();\n
 * Should return the name of the database table in which all the
 * primary keys of records for storing instances of Derived appear
 * in the database. Should have no side effects.
 *
 * static std::string Derived::primary_key_name();\n
 * Should return the name of the primary key column of the table
 * named by primary_table_name(). Should have no side effects.
 * (Note the primary key must be an autoincrementing primary key.)
 *
 *
 * <b>Template parameters</b>
 *
 * @param Derived The derived class. Derived should inherit publicly
 * from PersistentObject<Derived, Connection> per the Curiously Recurring
 * Template Pattern (CRTP).
 *
 * @param Connection The type of the database connection through which
 * instances of Derived will be persisted to the database. Connection
 * should be a class derived from sqloxx::DatabaseConnection.
 * 
 * @todo Go through all the client classes in Phatbooks and ensure the
 * do_save... functions in each are atomic with respect to
 * the in-memory objects, and conform to the restrictions detailed in the
 * PersistentObject API documentation. (Note I have already done this
 * for \e load functions.)
 *
 * @todo If Sqloxx is ever moved to a separate library, then the documentation
 * for PersistentObject should include code for an exemplary derived class.
 *
 * @todo Centralize list of preconditions in a single location.
 *
 * @todo Have a single location for documenting use of Sqloxx holistically.
 */
template<typename Derived, typename Connection>
class PersistentObject
{
public:

	friend class Handle<Derived>;

	typedef sqloxx::Id Id;
	typedef sqloxx::HandleCounter HandleCounter;
	typedef sqloxx::IdentityMap<Derived, Connection> IdentityMap;


	/**
	 * Destructor.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	virtual ~PersistentObject();

	/**
	 * Preconditions:\n
	 * Derived::primary_key_name() and Derived::primary_table_name()
	 * must be defined as static functions that, without side-effects,
	 * return a std::string being, respectively, the name of the
	 * primary key column
	 * for type Derived as it is stored in the database, and the name
	 * of the database table in which instances of Derived are stored
	 * by primary key.
	 *
	 * @returns true if and only if an object with p_id as its
	 * primary key exists in
	 * the database to which p_database_connection is connected.
	 * Note the database is always checked, not the
	 * cache.
	 *
	 * @throws InvalidConnection if the database connecton is
	 * invalid.
	 *
	 * @throws std::bad_alloc in case of memory allocation failure
	 * (very unlikely)
	 *
	 * @throws SQLiteException (or derivative thereof) in case of a
	 * SQLite error during
	 * execution. (This is extremely unlikely, but cannot be entirely
	 * ruled out.)
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	static bool exists(Connection& p_database_connection, Id p_id);

	/**
	 * Preconditions:\n
	 * The destructor of Derived must be non-throwing;\n
	 * We have handled this object only via a Handle<Derived>, with
	 * the Handle<Derived> having been copied or assigned from another
	 * Handle<Derived>, or obtained by a call to
	 * IdentityMap<Derived, Connection>::provide_handle(...);\n
	 * If the object has an id, the id corresponds to the primary
	 * key of an object of this type that exists in the database;\n
	 * do_save_existing() and do_save_new() must be defined in such
	 * a way that they do not change the logical state of the object being
	 * saved, or of any other objects, but have side-effects only
	 * in respect of the database;\n
	 * do_save_existing() and do_save_new() must be been defined so that
	 * if they fail, they throw std::exception or an exception derived
	 * therefrom;\n
	 * Every setter and getter method defined in the Derived class must have
	 * a call to load() as its first statement (see below for
	 * explanation);\n
	 * The primary_table_name() static function must be defined in the
	 * class Derived, and must simply return a std::string, being the
	 * name of the table in which instances of Derived are stored
	 * using an autoincrementing integer primary key (using the
	 * built-in SQLite "autoincrement" qualifier);\n
	 * Derived::do_ghostify() must be defined so as to be non-throwing;\n
	 * Derived::do_load() preconditions must be met (see documentation
	 * for load());
	 * 
	 * The result of calling save() depends on whether the in-memory
	 * object has an id.

	 * (1) <b>Object has id</b>
	 *
	 * If the object has an id - i.e. if it corresponds to an object already
	 * existent in the database - then
	 * calling save() will result in the object in the database being updated
	 * with
	 * the state of the current in-memory object.
	 * This is done by a call
	 * to virtual function do_save_existing(), which much be defined
	 * in the class Derived. The base method save() takes care of wrapping
	 * the call to do_save_existing() as a single SQL transaction by calling
	 * begin_transaction() and end_transaction() on the database connection.
	 * 
	 * <em>Important:</em> Before calling do_save_existing(), and before
	 * beginning the
	 * SQL transaction, the base save() method first ensures that the object
	 * is in a fully loaded state (as we don't want to save a partial
	 * object to the database). This is done via a call to load().
	 * If the object is <em>in a ghost state and has an id</em> at this point,
	 * then the entire
	 * state of the in-memory object will be overwritten by the state of
	 * the in-database object, and \e then the in-memory object will be
	 * saved - with the net result being that any changes to the in-memory
	 * object are lost, and the in-database object remains unchanged!
	 * If, on the other, hand, the object is in a fully loaded state at
	 * the point save() is called, then the call to load() has no effect.
	 * (The call to load() also has no effect if the object doesn't yet have
	 * an id; but that's not relevant here as we're considering the case
	 * where the object has an id.)
	 * The upshot of all this is that, in order to make sure that
	 * changes to the in-memory object remain in the in-memory object and
	 * are subsequently written to the database
	 * when save() is called, you should always call load() as
	 * the \e first statement in the implementation of any \e setter method in
	 * the Derived class.
	 * 
	 * (2) <b>Object does not have id</b>
	 *
	 * If the object does not have an id - i.e. if it does not correspond or
	 * purport to correspond to an object already saved to the database - then
	 * calling save() will result in the in-memory object being saved to the
	 * database as an additional item, rather than overwriting existing data.
	 * In other words, a new record will be created in the database.
	 * This is done
	 * via a call to virtual function do_save_new(), which must be defined in
	 * the class Derived. The base save() function takes care of wrapping
	 * this call as a SQL transaction. The base function also takes care of:
	 * assigning an id to the newly saved object in the database; recording
	 * this id in the in-memory object; and notifying the IdentityMap<Derived>
	 * (i.e. the "cache") for this object, that it has been saved and assigned
	 * its id.
	 *
	 * After saving the object as above, whether via (1) or (2), the in-memory
	 * object is marked internally as being in a fully loaded, i.e.
	 * "complete" state.
	 *
	 * In defining do_save_new(), the class Derived should ensure that a call
	 * to do_save_new() results in a \e complete object of its type being
	 * inserted into the database. The semantics of save() here only make
	 * sense if this is the case. The Sqloxx framework does not provide for
	 * the saving of objects "a bit at a time".
	 *
	 * @throws TableSizeException if the object does not have an id, but
	 * the greatest primary key value already in the primary table for the
	 * type Derived is
	 * the maximum possible value for the type Id, so that another row
	 * could not be inserted without overflow.
	 * 
	 * @throws std::bad_alloc in the unlikely event of memory allocation
	 * failure during execution.
	 *
	 * @throws TransactionNestingException if the maximum transaction
	 * nesting level of the DatabaseConnection has been reached (extremely
	 * unlikely). (May be thrown under either (1) or (2).)
	 *
	 * @throws InvalidConnection if the DatabaseConnection is
	 * invalid. (May be thrown under either (1) or (2).)
	 *
	 * @throws UnresolvedTransactionException if there is failure in
	 * the process of committing the database transaction, or if there is
	 * some other failure, followed by a failure in the process of
	 * \e formally cancelling the database transaction. If this is
	 * thrown (which is extremely unlikely), it is recommended that the
	 * application be gracefully terminated. The database transaction
	 * \e will be fully rolled back, but further transaction during the
	 * same application session may jeopardize that situation.
	 *
	 * May also throw other derivatives of DatabaseException if there is
	 * a failure finding the next primary key value for the object in case
	 * it doesn't already have an id. This should not occur except in the
	 * case of a corrupt database, or a memory allocation error (very
	 * unlikely).
	 *
	 * May also throw exceptions from do_save_new() and/or do_save_exising(),
	 * depending on how those functions are defined in the derived class.
	 *
	 * Exception safety: <em>basic guarantee</em>. Possible outcomes
	 * from calling save() are as follows -\n
	 *  (a) Complete success;\n
	 *  (b) Failure with exception thrown and no effect on program
	 *  state;\n
	 *  (c) If the object has an id, save() may fail and throw an
	 *  exception but with the object left in a
	 * ghost state.
	 * In either (b) or (c), it is possible that failure may occur
	 * with UnresolvedTransactionException being thrown. If this occurs,
	 * it is recommended that the application session be gracefully
	 * terminated.
	 * Note that (b) and (c) are functionally equivalent to one another
	 * as far as client
	 * code is concerned, providing the preconditions are met, and in
	 * particular, providing client Derived class always calls load() as
	 * the first statement of any getter.
	 */
	void save();

	/**
	 * Delete an object of type Derived from the database. The
	 * function will also inform the IdentityMap in which this
	 * object is cached, and the cache will update itself accordingly.
	 * After calling remove(), the object will no longer have an id;
	 * however its other attributes will be unaltered (assuming the
	 * Derived class doesn't define do_remove() in such a way as to
	 * alter them - see below).
	 *
	 * Preconditions:\n
	 * If the default implementation of do_remove() is not redefined
	 * by the class Derived, then the preconditions of do_remove()
	 * must be satisfied (see separate documentation for do_remove());\n
	 * If do_remove() is redefined by Derived, then it should offer the
	 * strong guarantee, i.e. be atomic, in respect of the state of
	 * the in-memory objects (but note, the base remove() method takes
	 * care of wrapping the implementation as a SQL transaction, so
	 * in general, do_removed() doesn't need to worry about atomicity
	 * in regards to the database);\n
	 * Derived::do_ghostify() should be defined so as to adhere to the
	 * preconditions detailed in the documentation for ghostify();\n and
	 * Getters and setters in Derived should always call load() as their
	 * first statement.
	 *
	 * @throws std::bad_alloc in the unlikely event of mememory allocation
	 * failure during execution.
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws TransactionNestingException if the maximum transaction
	 * nesting level of the DatabaseConnection has been reached (extremely
	 * unlikely).
	 *
	 * @throws UnresolvedTransactionException if there is failure in
	 * the process of committing the database transaction, or if there is
	 * some other failure, followed by a failure in the process of
	 * \e formally cancelling the database transaction. If this is
	 * thrown (which is extremely unlikely), it is recommended that the
	 * application be gracefully terminated. The database transaction
	 * \e will be fully rolled back, but further transaction during the
	 * same application session may jeopardize that situation.
	 *
	 * Exception safety:<em>basic guarantee</em>. If an exception other
	 * than UnresolvedTransactionException is thrown, then the
	 * application state will be effectively rolled back, and although the
	 * object may be left in a ghost state, this should require no
	 * special handling by the client code provided the preconditions are
	 * met. If UnresolvedTransactionException is thrown, then, provided
	 * the application exits the current session without executing any
	 * further database transactions, the application and database state
	 * will be in a state of having been effectively rolled back, when the
	 * next session commences.
	 */
	void remove();

	/**
	 * @returns the id of the object, if it has one.
	 *
	 * @throws jewel::UninitializedOptionalException if the object doesn't
	 * have an id.
	 * 
	 * Exception safety: <em>strong guarantee</em>.
	 */
	Id id() const;

	/**
	 * @returns \e true if this instance of PersistentObject has
	 * an valid id; otherwise returns \e false.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	bool has_id() const;

	/**
	 * Reverts the object to a "ghost state". This is a state in
	 * which only certain member variables (typically, only the id)
	 * are initialized. This is done by calling the private virtual
	 * function do_ghostify(). This must be defined by class Derived.
	 * Then, the base ghostify() method marks the object as being in a
	 * "ghost" state.
	 *
	 * Derived::do_ghostify() should be defined in such a way that,
	 * when executed, the object is put into such a state that, the
	 * next time load() is called, the object can be fully reloaded to
	 * a valid loaded state without any issues of duplication or etc.
	 * For example, if one of the member variables of Derived is a
	 * vector, and if loading the object involves pushing elements
	 * onto the vector, then do_ghostify() should ensure that the
	 * vector is emptied, so that after load() is called next, the
	 * object contains only one lot of elements.
	 *
	 * <em>It is strongly recommended that do_ghostify() be defined
	 * such as to provide the nothrow guarantee. This makes it
	 * easier for certain other functions to maintain exception-safety.
	 * </em>
	 *
	 * Exception safety: <em>nothrow guarantee</em>, provided the
	 * Derived::do_ghostify() method is non-throwing.
	 *
	 * @todo Determine if this really needs to be public.
	 */
	void ghostify();

	/**
	 * Provides access to get and set m_cache_key, to
	 * to IdentityMap<Derived, Connection>.
	 */
	class CacheKeyAttorney
	{
	public:
		friend class sqloxx::IdentityMap<Derived, Connection>;
	private:
		static void set_cache_key(Derived& p_obj, Id p_cache_key)
		{
			p_obj.set_cache_key(p_cache_key);
			return;
		}
		static Id cache_key(Derived& p_obj)
		{
			return jewel::value(p_obj.m_cache_key);
		}
	};
	
	friend class CacheKeyAttorney;

	/**
	 * Controls access to functions that monitor the number of
	 * Handle<T> instances pointing to a given instance of
	 * PersistentObject<T, Connection>, deliberately restricting
	 * this access to IdentityMap<Derived, Connection>
	 */
	class HandleMonitorAttorney
	{
	public:
		friend class sqloxx::IdentityMap<Derived, Connection>;
	private:
		static bool is_orphaned(Derived const& p_obj)
		{
			return p_obj.is_orphaned();
		}
		static bool has_high_handle_count(Derived const& p_obj)
		{
			return p_obj.has_high_handle_count();
		}
	};

	friend class HandleMonitorAttorney;


protected:

	/**
	 * This should only be called by Derived.
	 *
	 * Create a PersistentObject that corresponds (or purports to correspond)
	 * to one that already exists in the database.
	 *
	 * @param p_identity_map IdentityMap with
	 * which the PersistentObject is associated.
	 *
	 * @param p_id the id of the object as it exists in the database. This
	 * presumably will be, or correspond directly to, the primary key.
	 *
	 * Preconditions:\n
	 * Note that even if there is no corresponding object in the database for
	 * the given value p_id, this constructor will still proceed without
	 * complaint. The constructor does not actually perform any checks on the
	 * validity either of p_database_connection or of p_id. The caller should
	 * be sure, before calling this function, that there exists in the
	 * database a row representing an instance of the Derived type, with p_id
	 * as it primary key. If no such row exists, then UNDEFINED BEHAVIOUR will
	 * result, including the possibility of silent or delayed corruption of
	 * data.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	PersistentObject
	(	IdentityMap& p_identity_map,
		Id p_id
	);

	/** 
	 * This should only be called by Derived.
	 *
	 * Create a PersistentObject that does \e not correspond to
	 * one that already exists in the database.
	 *
	 * @param p_identity_map IdentityMap with which the
	 * PersistentObject is to be associated.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	explicit
	PersistentObject
	(	IdentityMap& p_identity_map
	);


	/**
	 * Calls the derived class's implementation
	 * of do_load(), if and only if the object is not already
	 * loaded. If the object is already loaded, it does nothing.
	 * Also if the object does not have an id,
	 * then this function does nothing, since there would be nothing
	 * to load.
	 *
	 * Preconditions:\n
	 * In defining do_load(), the Derived class should throw an instance
	 * of std::exception (which may be an instance of any exception class
	 * derived therefrom) in the event that the load fails;\n
	 * do_load() should not perform any write operations on the database,
	 * and should provide 
	 * the strong exception-safety guarantee;\n
	 * The Derived class should define do_ghostify() according to the
	 * preconditions specified in the documentaton of ghostify(); and\n
	 * The destructor of Derived must be non-throwing.
	 *
	 * Note the implementation is wrapped as a transaction
	 * by calls to begin_transaction and end_transaction
	 * methods of the DatabaseConnection. This is taken care of by the
	 * base \e load method.
	 *
	 * The following exceptions may be thrown regardless of how
	 * do_load() is defined:
	 *
	 * @throws TransactionNestingException in the event that the maximum
	 * level of transaction nesting for the database connection has been
	 * reached. (This is extremely unlikely.) If this occurs \e before
	 * do_load() is entered, the object will be as it was before the
	 * function was called.
	 * 
	 * @throws InvalidConnection in the event that the database connection is
	 * invalid at the point the \e load function is entered. If this occurs,
	 * the object will be as it was before this function was called.
	 *
	 * @throws std::bad_alloc in the event of memory allocation failure
	 * during execution.
	 *
	 * @throws UnresolvedTransactionException if there is failure in
	 * the process of committing the database transaction, or if there is
	 * some other failure, followed by a failure in the process of
	 * \e formally cancelling the database transaction. If this is
	 * thrown (which is extremely unlikely), it is recommended that the
	 * application be gracefully terminated. The database transaction
	 * \e will be fully rolled back, but attempting further transactions
	 * during the same application session may jeopardize that situation.
	 *
	 * Exception safety: <em>basic guarantee</em>, provided the
	 * preconditions are met. Either there will be complete success,
	 * or the object will be left in a ghost state, functionally
	 * equivalent, as far as client code is concerned, to the state
	 * it was in prior to load() being called. The possibility of
	 * UnresolvedTransactionException means the strong guarantee cannot
	 * be provided, however (see above).
	 */
	void load();

	/**
	 * Copy constructor is deliberately protected. Copy construction does
	 * not make much semantic sense, as each instance of PersistentObject is
	 * supposed to represent a \e unique object in the database, with a
	 * unique id. However we provide it to derived classes, who may wish
	 * to use it in, for example, copy-and-swap operations.
	 *
	 * Exception safety: <em>nothrow guarantee</em> (though derived classes'
	 * copy constructors might, of course, throw).
	 */
	PersistentObject(PersistentObject const& rhs);

	/**
	 * Swap function. Does what you expect. This swaps the base part of
	 * the object only.
	 *
	 * Exception safety: <em>nothrow guarantee</em>
	 */
	void swap_base_internals(PersistentObject& rhs);

	/**
	 * @returns a reference to the database connection with which
	 * this instance of PersistentObject is associated. This is where the
	 * object will be loaded from or saved to, as the case may be.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	Connection& database_connection() const;

	/**
	 * @returns the id that would be assigned to this instance of
	 * PersistentObject when saved to the database. This uses SQLite's
	 * built-in auto-incrementing primary key.
	 *
	 * @throws sqloxx::LogicError in the event this instance already has
	 * an id.
	 *
	 * @throws sqloxx::TableSizeException if the greatest primary key value
	 * already in the table (i.e. the table into which this instance of
	 * PersistentObject would be persisted) is the maximum value for the
	 * type \e Id, so that another row could not be inserted without overflow.
	 *
	 * @throws std::bad_alloc is the unlikely event that memory allocation
	 * fails during execution.
	 *
	 * @throws sqloxx::DatabaseException, or a derivative therefrom, may
	 * be thrown if there is some other
	 * error finding the next primary key value. This should not occur except
	 * in the case of a corrupt database, or a memory allocation error
	 * (extremely unlikely), or the database connection being invalid
	 * (including because not yet connected to a database file).
	 * The particular child class of DatabaseException thrown will depend
	 * on the type of error, e.g. InvalidConnection will be thrown
	 * in the event of an invalid database connection.
	 *
	 * Exception safety: the default implementation offers the
	 * <em>strong guarantee</em> - providing the virtual
	 * function primary_table_name() does nothing odd but simply returns a
	 * std::string as would be expected.
	 */
	Id prospective_key() const;

	/**
	 * This function is called by remove(). For that function, and the
	 * role of do_remove() within that function, see the separate
	 * documentation for remove().
	 *
	 * The following relates the default implementation of do_remove()
	 * provided by PersistentObject<Derived, Connection>.
	 *
	 * The default implementation of this function will simply delete
	 * the row with the primary key returned by id(), in the table
	 * named by Derived::primary_table_name().
	 *
	 * Preconditions:\n
	 * Derived::primary_table_name() must be defined by Derived so as
	 * to return the name of the table in which all instances of Derived
	 * are stored with their primary key, as a std::string; and\n
	 * The primary key must be a single column primary key, the name
	 * of which is returned by Derived::primary_key_name().
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws std::bad_alloc in the unlikely event of a memory
	 * allocation failure in execution.
	 *
	 * Exception safety:<em>strong guarantee<em>.
	 */
	virtual void do_remove();

private:

	 // Deliberately unimplemented. Assignment doesn't make much semantic
	 // sense for a PersistentObject that is supposed to
	 // represent a \e unique object in the database with a unique id.
	PersistentObject& operator=(PersistentObject const& rhs);

	virtual void do_load() = 0;
	virtual void do_save_existing() = 0;
	virtual void do_save_new() = 0;
	virtual void do_ghostify() = 0;
	void increment_handle_counter();

	/**
	 * @returns true if and only if there are no Handle<Derived>
	 * instances pointing to this object.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	bool is_orphaned() const;
	
	/**
	 * @returns true if and only if we are dangerously close to reaching
	 * the maximum value of HandleCounter.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	bool has_high_handle_count() const;

	/**
	 * Called by Handle<Derived> via PersistentObjectHandleAttorney
	 * to advise the underlying
	 * object that a handle pointing to it has been constructed (not copy-
	 * constructed, but ordinarily constructed).
	 * 
	 * @throws sqloxx::OverflowException if the maximum value
	 * for type HandleCounter has been reached, such that additional Handle<T>
	 * cannot be safely counted. On the default type for HandleCounter,
	 * this should be extremely unlikely.
	 *
	 * Exception safety: <em>strong guarantee</em>
	 */
	void notify_handle_construction();

	/**
	 * Called by Handle<Derived> via PersistentObjectHandleAttorney to
	 * advise the underlying
	 * object that a handle pointing to it has been copy-constructed.
	 * 
	 * @throws sqloxx::OverflowException if the maximum value of
	 * HandleCounter has been reached such that additional handles
	 * cannot be safely counted. On the default type for HandleCounter,
	 * this should be extremely unlikely.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	void notify_handle_copy_construction();

	/**
	 * Called by Handle<Derived> via PersistentObjectHandleAttorney
	 * to advise the
	 * underlying object that a handle pointing to it has appeared
	 * as the right-hand operand of an assignment operation.
	 *
	 * @throws sqloxx::OverflowException if the maximum value of
	 * HandleCounter has been reached such that additional handles
	 * cannot be safely counted. On the default type for HandleCounter,
	 * this should be extremely unlikely.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	void notify_rhs_assignment_operation();
	
	/**
	 * Called by Handle<Derived> via PersistentObjectHandleAttorney
	 * to advise the
	 * underlying object that a Handle pointing to it has appeared
	 * as the right-hand operand of an assignment operation.
	 *
	 * Preconditions:\n
	 * This function should only be called from Handle<Derived> to
	 * notify that a Handle pointing to this instance of Derived
	 * appears on the left side of an assignment operation;\n
	 * This instance of Derived must have been handled throughout
	 * its life only via instances of Handle<Derived>, that have been obtained
	 * from a single instance of IdentityMap<Derived, Connection>
	 * via calls to the IdentityMap API, or else have been copied from other
	 * instances of Handle<Derived>; and\n
	 * The destructor of derived must be non-throwing.
	 *
	 * Exception safety: <em>nothrow guarantee</em> is offered, providing
	 * the preconditions are met.
	 */
	void notify_lhs_assignment_operation();

	/**
	 * Called by Handle<Derived> via PersistentObjectHandleAttorney,
	 * to advise the underlying
	 * object that a handle pointing to it has been destructed.
	 *
	 * Preconditions: as for notify_lhs_assignment_operation().
	 *
	 * Exception safety: as for notify_lhs_assignment_operation().
	 */
	void notify_handle_destruction();

	/**
	 * Called by IdentityMap<Derived, Connection> via CacheKeyAttorney to
	 * provide a "cache
	 * key" to the object. The cache key is used by IdentityMap to
	 * identify the object in its internal cache. Every object created by
	 * the IdentityMap will have a cache key, even if it doesn't have an id.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void set_cache_key(Id p_cache_key);

	/**
	 * Decrements handle counter and notifies the IdentityMap if it
	 * reaches 0.
	 *
	 * @throws sqloxx::OverflowException if m_handle_counter is less
	 * than 1 when this function is called.
	 *
	 * Preconditions: we must know that the object cached is in the
	 * IdentityMap under m_cache_key. If the object has been managed
	 * throughout its life by (a single instance of) IdentityMap,
	 * and has only ever been
	 * accessed via instances of Handle<Derived>, then we know this is
	 * the case. Also, the destructor of Derived must be non-throwing.
	 *
	 * Exception safety: <em>nothrow guarantee</em>, providing the
	 * object is cached in the IdentityMap under m_cache_key, and
	 * the destructor of Derived is non-throwing.
	 */
	void decrement_handle_counter();

	enum LoadingStatus
	{
		ghost = 0,
		loading,
		loaded
	};

	
	// Data members

	IdentityMap& m_identity_map;

	// Represent primary key in database. If the object does not correspond to
	// and does not purport to correspond to any record in the database, then
	// m_id in unitialized.
	boost::optional<Id> m_id;
	
	// Represents the identifier, in the IdentityMap<Derived> for
	// m_database_connection, of an instance of Derived. The
	// IdentityMap<Derived> can look up a PersistentObject either via its id
	// (which corresponds to its primary key in the database), or via
	// its cache_key. PersistentObject instances that are newly created and
	// have not yet been saved to the database will not have an id (i.e. m_id
	// will be in an uninitialized state), however these may still be managed
	// by the IdentityMap, and so still need a means for the IdentityMap to
	// identify them in their internal cache. Hence the need for a cache_key
	// distinct from the id.
	boost::optional<Id> m_cache_key;

	LoadingStatus m_loading_status;
	HandleCounter m_handle_counter;
};


/**
 * Controls access to functions used
 * by Handle<T> to notify the underlying object of
 * various events for "bookkeeping" purposes.
template <typename T>
class PersistentObjectHandleAttorney
{
// This class would have been better nested in PersistentObject,
// were it not for the presence of the template parameter Connection
// of PersistentObject, which makes this problematic.
public:
	friend class Handle<T>;
private:
	static void notify_handle_construction(T& p_obj)
	{
		p_obj.notify_handle_construction();
		return;
	}
	static void notify_handle_copy_construction(T& p_obj)
	{
		p_obj.notify_handle_copy_construction();
		return;
	}
	static void notify_lhs_assignment_operation(T& p_obj)
	{
		p_obj.notify_lhs_assignment_operation();
		return;
	}
	static void notify_rhs_assignment_operation(T& p_obj)
	{
		p_obj.notify_rhs_assignment_operation();
		return;
	}
	static void notify_handle_destruction(T& p_obj)
	{
		p_obj.notify_handle_destruction();
		return;
	}
};  // PersistentObjectAttorney
*/



template
<typename Derived, typename Connection>
PersistentObject<Derived, Connection>::PersistentObject
(	IdentityMap& p_identity_map,
	Id p_id
):
	m_identity_map(p_identity_map),
	m_id(p_id),
	m_loading_status(ghost),
	m_handle_counter(0)
{
}

template
<typename Derived, typename Connection>
PersistentObject<Derived, Connection>::PersistentObject
(	IdentityMap& p_identity_map	
):
	m_identity_map(p_identity_map),
	m_loading_status(ghost),
	m_handle_counter(0)
	// Note m_cache_key is left unitialized. It is the responsibility
	// of IdentityMap<Derived> to call set_cache_key after construction,
	// before providing a Handle to a newly created Derived instance.
{
}

template
<typename Derived, typename Connection>
PersistentObject<Derived, Connection>::~PersistentObject()
{
}

template
<typename Derived, typename Connection>
bool
PersistentObject<Derived, Connection>::exists
(	Connection& p_database_connection,
	Id p_id
)
{
	// Could throw std::bad_alloc
	static std::string const text =
		"select * from " +
		Derived::primary_table_name() +
		" where " +
		Derived::primary_key_name() +
		" = :p";
	// Could throw InvalidConnection or SQLiteException
	SQLStatement statement(p_database_connection, text);
	// Could throw InvalidConnection or SQLiteException
	statement.bind(":p", p_id);
	// Could throw InvalidConnection or SQLiteException
	return statement.step();
}


template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::load()
{
	while (m_loading_status == loading)
	{
		// Wait
	}
	if (m_loading_status == ghost && has_id())
	{
		DatabaseTransaction transaction(database_connection());
		m_loading_status = loading;
		try
		{
			do_load();	
			transaction.commit();
		}
		catch (std::exception&)
		{
			ghostify();
			transaction.cancel();
			throw;
		}
		m_loading_status = loaded;
	}
	return;
}


template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::save()
{
	assert (m_cache_key);  // precondition
	if (has_id())  // nothrow
	{
		// strong guarantee, under preconditions of do_load (see load())
		load(); 

		// strong guarantee
		DatabaseTransaction transaction(database_connection());
		try
		{
			do_save_existing();  // Safety depends on Derived
			transaction.commit();  // Strong guarantee
		}
		catch (std::exception&)
		{
			ghostify();  // nothrow (assuming preconditions met)
			transaction.cancel();
			throw;
		}
	}
	else
	{
		Id const allocated_id = prospective_key();  // strong guarantee
		DatabaseTransaction transaction(database_connection());// strong guar.
		try
		{
			do_save_new();  // Safety depends on Derived

			// strong guarantee
			IdentityMap::PersistentObjectAttorney::register_id
			(	m_identity_map,
				*m_cache_key,
				allocated_id
			);
			try
			{
				transaction.commit(); // strong guarantee
			}
			catch (std::exception&)
			{
				// nothrow (assuming preconditions met)
				IdentityMap::PersistentObjectAttorney::deregister_id
				(	m_identity_map,
					allocated_id
				);
				throw;
			}
		}
		catch (std::exception&)
		{
			jewel::clear(m_id);  // nothrow
			transaction.cancel();
			throw;
		}
		m_id = allocated_id; // nothrow
	}
	m_loading_status = loaded;  // nothrow
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::remove()
{
	if (has_id())
	{
		DatabaseTransaction transaction(database_connection());// strong guar.
		try
		{
			do_remove(); // safety depends on derived. by default strong guar.
			transaction.commit();  // strong guarantee
		}
		catch (std::exception&)
		{
			ghostify();  // nothrow, providing preconditions met
			transaction.cancel();
			throw;
		}

		// nothrow (conditional)
		IdentityMap::PersistentObjectAttorney::uncache_object
		(	m_identity_map,
			*m_cache_key
		);

		jewel::clear(m_id);  // nothrow
	}
	return;
}


template
<typename Derived, typename Connection>
Id
PersistentObject<Derived, Connection>::id() const
{
	return jewel::value(m_id);
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::set_cache_key(Id p_cache_key)
{
	m_cache_key = p_cache_key;
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::notify_handle_construction()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::notify_handle_copy_construction()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::notify_rhs_assignment_operation()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::notify_lhs_assignment_operation()
{
	decrement_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::notify_handle_destruction()
{
	decrement_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::increment_handle_counter()
{
	if (m_handle_counter == std::numeric_limits<HandleCounter>::max())
	{
		throw OverflowException
		(	"Handle counter for PersistentObject instance has reached "
			"maximum value and cannot be safely incremented."
		);
	}
	++m_handle_counter;
	return;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::decrement_handle_counter()
{
	switch (m_handle_counter)
	{
	case 1:
		--m_handle_counter;
		// Will not throw, provided the destructor of Derived
		// is non-throwing, and the object is saved in the cache
		// under m_cache_key.
		if (m_cache_key)
		{
			IdentityMap::PersistentObjectAttorney::notify_nil_handles
			(	m_identity_map,
				*m_cache_key
			);
		}
		break;
	case 0:
		// Do nothing
		break;
	default:
		assert (m_handle_counter > 1);
		--m_handle_counter;
	}
	return;
}

template
<typename Derived, typename Connection>
Connection&
PersistentObject<Derived, Connection>::database_connection() const
{
	return m_identity_map.connection();
}

template
<typename Derived, typename Connection>
Id
PersistentObject<Derived, Connection>::prospective_key() const
{
	if (has_id())
	{
		throw LogicError
		(	"Object already has id so prospective_key does not apply."
		);
	}
	return next_auto_key<Connection, Id>
	(	database_connection(),
		Derived::primary_table_name()
	);
}


template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::do_remove()
{
	// primary_table_name() might throw std::bad_alloc (strong guar.).
	// primary_key_name() might throw might throw InvalidConnection or
	// std::bad_alloc.
	//
	std::string const statement_text =
		"delete from " + Derived::primary_table_name() + " where " +
		Derived::primary_key_name() + " = :p";
	
	// Might throw InvalidConnection or std::bad_alloc
	SQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());  // Might throw InvalidConnection
	// throwing above this point will have no effect

	statement.step_final();  // Might throw InvalidConnection
	return;
}


template
<typename Derived, typename Connection>
bool
PersistentObject<Derived, Connection>::has_id() const
{
	// Relies on the fact that m_id is a boost::optional<Id>, and
	// will convert to true if and only if it has been initialized.
	return m_id;
}

template
<typename Derived, typename Connection>
bool
PersistentObject<Derived, Connection>::is_orphaned() const
{
	return m_handle_counter == 0;
}

template
<typename Derived, typename Connection>
bool
PersistentObject<Derived, Connection>::has_high_handle_count() const
{
	static HandleCounter const safe_limit =
		std::numeric_limits<HandleCounter>::max() - 2;
	return m_handle_counter >= safe_limit;
}

template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::
ghostify()
{
	do_ghostify();
	m_loading_status = ghost;
	return;
}

template
<typename Derived, typename Connection>
PersistentObject<Derived, Connection>::PersistentObject
(	PersistentObject const& rhs
):
	m_identity_map(rhs.m_identity_map),
	m_id(rhs.m_id),
	m_cache_key(rhs.m_cache_key),
	m_loading_status(rhs.m_loading_status)
{
}
		
template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::swap_base_internals
(	PersistentObject& rhs
)
{
	IdentityMap temp_id_map = rhs.m_identity_map;
	boost::optional<Id> temp_id = rhs.m_id;
	boost::optional<Id> temp_cache_key = rhs.m_cache_key;
	LoadingStatus temp_loading_status = rhs.m_loading_status;

	rhs.m_identity_map = m_identity_map;
	rhs.m_id = m_id;
	rhs.m_cache_key = m_cache_key;
	rhs.m_loading_status = m_loading_status;

	m_identity_map = temp_id_map;
	m_id = temp_id;
	m_cache_key = temp_cache_key;
	m_loading_status = temp_loading_status;

	return;
}



}  // namespace sqloxx



#endif  // GUARD_persistent_object_hpp






