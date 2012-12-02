#ifndef GUARD_persistent_object_hpp
#define GUARD_persistent_object_hpp

#include "database_transaction.hpp"
#include "general_typedefs.hpp"
#include "identity_map.hpp"
#include "next_auto_key.hpp"
#include "shared_sql_statement.hpp"
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
 * USE OF IDENTITY MAP PATTERN
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
 * LAZY LOADING VIA GHOST PATTERN
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
 * The following functions need to be provided with definitions
 * provided in the derived class:
 *
 * <b>static std::string primary_table_name();</b>\n
 * Should return name of table in which instances of the derived class
 * are persisted in the database. If instance are persisted across
 * multiple tables, this function should return the "primary table",
 * i.e. a table containing the primary key for this class, such that
 * every persisted instance of this class has a row in this table.
 *
 * <b>virtual void do_load() = 0;</b>\n
 * See documentation of load() function.
 *
 * <b>virtual void do_save_existing() = 0;</b>\n
 * See documentation for save_existing() function.
 *
 * <b>virtual void do_save_new() = 0;</b>\n
 * See documentation for save_new() function.
 *
 *
 * TEMPLATE PARAMETERS
 *
 * @param Derived The derived class. Derived should inherit publicly
 * from PersistentObject<Derived, Connection> per the Curiously Recurring
 * Template Pattern (CRTP).
 *
 * @param Connection The type of the database connection through which
 * instances of Derived will be persisted to the database. Connection
 * should be a class derived from sqloxx::DatabaseConnection.
  * 
 * @todo Provide for atomic saving (not just of
 * SQL execution, but of the actual alteration of the in-memory objects).
 * Go through all the client classes in Phatbooks and ensure the
 * do_save... functions in each are atomic with respect to
 * the in-memory objects, and conform to the restrictions detailed in the
 * PersistentObject API documentation. (Note I have already done this
 * for \e load functions.)
 *
 * @todo If Sqloxx is ever moved to a separate library, then the documentation
 * for PersistentObject should include code for an exemplary derived class.
 */
template<typename Derived, typename Connection>
class PersistentObject
{
public:

	typedef sqloxx::Id Id;
	typedef sqloxx::HandleCounter HandleCounter;
	typedef sqloxx::IdentityMap<Derived, Connection> IdentityMap;

	/**
	 * This should only be called from IdentityMap<Derived, Connection>.
	 *
	 * Create a PersistentObject that corresponds (or purports to correspond)
	 * to one that already exists in the database.
	 *
	 * @param p_database_connection database connection with
	 * which the PersistentObject is associated.
	 *
	 * @param p_id the id of the object as it exists in the database. This
	 * presumably will be, or correspond directly to, the primary key.
	 *
	 * Note that even if there is no corresponding object in the database for
	 * the given value p_id, this constructor will still proceed without
	 * complaint. The constructor does not actually perform any checks on the
	 * validity either of p_database_connection or of p_id.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	PersistentObject
	(	IdentityMap& p_identity_map,
		Id p_id
	);

	/** 
	 * This should only be called from IdentityMap<Derived, Connection>.
	 *
	 * Create a PersistentObject that does \e not correspond to
	 * one that already exists in the database.
	 *
	 * @param p_database_connection database connection with which the
	 * PersistentObject is to be associated.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	explicit
	PersistentObject
	(	IdentityMap& p_identity_map
	);

	/**
	 * Destructor.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	virtual ~PersistentObject();

	/**
	 * The result of calling this function depends on whether the in-memory
	 * object has an id.
	 *
	 * (1) <b>Object has id</b>
	 *
	 * If the object has an id - i.e. if it corresponds, or purports to
	 * correspond, to an object already existent in the database - then
	 * calling save() will result in the object in the database being updated
	 * with
	 * the state of the current in-memory object. This is done by a call
	 * to virtual function do_save_existing(), which much be defined
	 * in the class Derived. The base method save() takes care of wrapping
	 * the call to do_save_existing() as a single SQL transaction by calling
	 * begin_transaction() and end_transaction on the database connection.
	 * Note that before calling do_save_existing(), and before beginning the
	 * SQL transaction, the base save() method first calls load().
	 * This ensures the object
	 * is not saved in a partial state. This call to load() only has
	 * effect if the object is in a partial ("ghost") state when it
	 * is called. If it is already in a fully loaded state, the call to
	 * load() will have no effect on the state of the in-memory object.
	 * However if it is in a "ghost" state when load() is called, then the
	 * entire state of the in-memory object will be overridden with the state
	 * of the in-database object.
	 * The upshot of this is that, in order to make sure that
	 * changes to the in-memory object remain in the in-memory object and
	 * are subsequently written to the database
	 * when save() is called, you should always call load() as
	 * the \e first statement in the implementation of any \e setter method in
	 * the derived class. This ensures ensures the in-memory object is in
	 * sync with the in-database object, prior to any changes being made to
	 * the in-memory object.
	 * 
	 * (2) <b>Object does not have id</b>
	 *
	 * If the object does not have an id - i.e. if it does not correspond or
	 * purport to correspond to an object already saved to the database - then
	 * calling save() will result in the in-memory object being saved to the
	 * database as an additional item, rather than overwriting existing data.
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
	 * object is marked as being in a loaded state. That is to say, the
	 * in-memory model of the object is marked internally as being in a
	 * complete and up-to-date state.
	 *
	 * In defining do_save_new(), the class Derived should ensure that a call
	 * to do_save_new() results in a \e complete object of its type being
	 * inserted into the database. The semantics of save() here only make
	 * sense if this is the case.
	 *
	 * The primary_table_name() static function must also
	 * be defined in the Derived class in order for this function
	 * to find an automatically generated id to assign to the object
	 * when saved. This should return a std::string being the name of
	 * the table housing the primary key for objects of this class stored
	 * in the database.
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
	 * nesting level of the DatabaseConnection has been reached (very
	 * unlikely). (May be thrown under either (1) or (2).)
	 *
	 * @throws InvalidConnection if the DatabaseConnection is
	 * invalid. (May be thrown under either (1) or (2).)
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
	 * Preconditions for exception-safety guarantee to be met:\n
	 * Derived::do_ghostify() must be non-throwing;\n
	 * Derived::do_load() must be strongly exception-safe;\n
	 * Object cache_key must be initialized;\n
	 * @todo Figure out other preconditions...
	 *
	 * @todo Figure out what other exceptions may be thrown, particularly
	 * in light of the call to IdentityMap<...>::register_id(...).
	 *
	 * @todo Testing, and reassessment of exceptions and exceptions safety.
	 */
	void save();

	/**
	 * @todo Documentation and testing.
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
	 * Should only be called by IdentityMap<Derived>. This assigns a "cache
	 * key" to the object. The cache key is used by IdentityMap<Derived> to
	 * identify the object in its internal cache. Every object created by
	 * the IdentityMap will have a cache key, even if it doesn't have an id.
	 *
	 * @todo The cache key should be able to be specified
	 * in the constructor. But we
	 * also don't want to confuse it with the other
	 * two-paramatered constructor! So that's why it's we have a
	 * separate function to set the cache key. But the context in
	 * which this is used is always going to be just after construction.
	 * So this feels a bit crappy. Is there a better way?
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 *
	 * @todo Test.
	 */
	void set_cache_key(Id p_cache_key);

	/**
	 * Should only be called by Handle<Derived>. To advise the underlying
	 * object that a handle pointing to it has been constructed (not copy-
	 * constructed, but ordinarily constructed).
	 * 
	 * @throws sqloxx::OverflowException if the maximum value
	 * for type HandleCounter has been reached, such that additional Handle<T>
	 * cannot be safely counted. On the default type for HandleCounter,
	 * this should be extremely unlikely.
	 *
	 * Exception safety: <em>strong guarantee</em>
	 *
	 * @todo Testing.
	 */
	void notify_handle_construction();

	/**
	 * Should only be called by Handle<Derived>. To advise the underlying
	 * object that a handle pointing to it has been copy-constructed.
	 * 
	 * @throws sqloxx::OverflowException if the maximum value of
	 * HandleCounter has been reached such that additional handles
	 * cannot be safely counted. On the default type for HandleCounter,
	 * this should be extremely unlikely.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *  
	 * @todo Testing.
	 */
	void notify_handle_copy_construction();

	/**
	 * Should only be called by Handle<Derived>. To advise the
	 * underlying object that a handle pointing to it has appeared
	 * as the right-hand operand of an assignment operation.
	 *
	 * @throws sqloxx::OverflowException if the maximum value of
	 * HandleCounter has been reached such that additional handles
	 * cannot be safely counted. On the default type for HandleCounter,
	 * this should be extremely unlikely.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 *
	 * @todo Test.
	 */
	void notify_rhs_assignment_operation();
	
	/**
	 * Should only be called by Handle<Derived>. To advise the
	 * underlying object that a handle pointing to it has appeared
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
	 *
	 * @todo Test.
	 */
	void notify_lhs_assignment_operation();

	/**
	 * Should only be called by Handle<Derived>, to advise the underlying
	 * object that a handle pointing to it has been destructed.
	 *
	 * Preconditions: as for notify_lhs_assignment_operation().
	 *
	 * Exception safety: as for notify_lhs_assignment_operation().
	 *
	 * @todo Test.
	 */
	void notify_handle_destruction();

	/**
	 * @returns \e true if this instance of PersistentObject has
	 * an valid id; otherwise returns \e false.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	bool has_id() const;
	
	/**
	 * Should only be called by IdentityMap<Derived>.
	 *
	 * @returns true if and only if there are no Handle<Derived>
	 * instances pointing to this object.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 *
	 * @todo Testing.
	 */
	bool is_orphaned() const;
	
	/**
	 * Should only be called from IdentityMap<Derived, Connection>.
	 *
	 * @returns true if and only if we are dangerously close to reaching
	 * the maximum value of HandleCountl.
	 *
	 * @todo Testing.
	 *
	 * @todo Do we need this? (Note IdentityMap calls it and presently
	 * no-one else does.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	bool has_high_handle_count() const;

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
	 * object contains only one "lot" of elements.
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
	 *
	 * @todo Testing.
	 */
	void ghostify();

protected:

	/**
	 * Calls the derived class's implementation
	 * of do_load(), if and only if the object is not already
	 * loaded. If the object is already loaded, it does nothing.
	 * Also if the object does not have an id,
	 * then this function does nothing, since there would be nothing
	 * to load.
	 *
	 * In defining do_load(), the derived class should throw an instance
	 * of std::exception (which may be an instance of any exception class
	 * derived therefrom) in the event that the load fails. If this
	 * is adhered to, and do_load() is implemented with the strong
	 * exception-safety guarantee, and do_load() does not perform any
	 * write operations on the database, or have other side-effects, then the
	 * \e load function will itself provide the strong exception safety
	 * guarantee.
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
	 * Exception safety: depends on how the derived class defines \e
	 * do_load(). See above.
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
	 *
	 * @todo Testing.
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
	 * function primary_table_name does nothing odd but simply returns a
	 * std::string as would be expected.
	 */
	Id prospective_key() const;

	
private:

	 // Deliberately unimplemented. Assignment doesn't make much semantic
	 // sense for a PersistentObject that is supposed to
	 // represent a \e unique object in the database with a unique id.
	PersistentObject& operator=(PersistentObject const& rhs);

	std::string primary_key_name();

	virtual void do_load() = 0;
	virtual void do_save_existing() = 0;
	virtual void do_save_new() = 0;
	virtual void do_ghostify() = 0;
	virtual void do_remove();
	void increment_handle_counter();

	/**
	 * Decrements handle counter and notifies the IdentityMap if it
	 * reaches 0.
	 *
	 * @throws sqloxx::OverflowException if m_handle_counter is less
	 * than 1 when this function is called.
	 *
	 * Precondition: we must know that the object cached is in the IdentityMap
	 * under m_cache_key. If the object has been managed throughout its
	 * life by (a single instance of) IdentityMap, and has only ever been
	 * accessed via instances of Handle<Derived>, then we know this is
	 * the case. Also, the destructor of Derived must be non-throwing.
	 *
	 * Exception safety: <em>nothrow guarantee</em>, providing the
	 * object is cached in the IdentityMap under m_cache_key,
	 * the destructor of Derived is non-throwing, and m_handle_counter is
	 * greater than 0. If we don't know whether the last condition is
	 * fulfilled, i.e. if m_handle_counter could be less than 1, then
	 * the <strong guarantee</em> is offered, provided the first two
	 * conditions are still met.
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
		}
		catch (std::exception&)
		{
			ghostify();
			transaction.cancel();
			throw;
		}
		try
		{
			transaction.commit();
			// Note this can't possibly throw TransactionNestingException
			// here, unless do_load() has done something perverse.
		}
		catch (InvalidConnection&)
		{
			// As do_load has already completed, the object in
			// memory should be non-corrupt and fully loaded. The fact that
			// the database connection is now invalid only affects the
			// database, not the in-memory object. The invalidity of the
			// database connection will presumably be detected and dealt with
			// the next time it is accessed. We therefore do NOT rethrow
			// here.
			//
			// WARNING Am I really comfortable with this?
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
	// Precondition
	assert (m_cache_key);

	// Possible outcomes, provided preconditions met:
	// Complete success;
	// Fails with no effect;
	// Fails and object left in ghost state (client requires no additional
	// recovery effort);
	// Fails 
	if (has_id())  // nothrow
	{
		load();  // strong guarantee, under certain conditions
		DatabaseTransaction transaction(database_connection()); // strong guarantee
		try
		{
			do_save_existing();  // Depends on Derived
			transaction.commit();  // Strong guarantee
		}
		catch (std::exception&)
		{
			ghostify();  // nothrow (assuming preconditions met)
			throw;
		}
	}
	else
	{
		Id const allocated_id = prospective_key();  // strong guarantee
		DatabaseTransaction transaction(database_connection());  // strong guarantee
		try
		{
			do_save_new();  // Depends on Derived
			m_identity_map.register_id(*m_cache_key, allocated_id);  // strong guarantee
			try
			{
				transaction.commit();  // Strong guarantee
			}
			catch (std::exception&)
			{
				// todo declare and define IdentityMap<...>::deregister_id(...),
				// and make it non-throwing.
				m_identity_map.deregister_id(m_cache_key);
				throw;
			}
		}
		catch (std::exception&)
		{
			ghostify();  // nothrow (assuming preconditions met)
			jewel::clear(m_id);
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
		DatabaseTransaction transaction(database_connection());
		do_remove();
		ghostify();
		transaction.commit();
		m_identity_map.uncache_object(*m_cache_key);	
		jewel::clear(m_id);  // Return id() to uninitialized state
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
inline
void
PersistentObject<Derived, Connection>::set_cache_key(Id p_cache_key)
{
	m_cache_key = p_cache_key;
	return;
}

template
<typename Derived, typename Connection>
inline
void
PersistentObject<Derived, Connection>::notify_handle_construction()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
inline
void
PersistentObject<Derived, Connection>::notify_handle_copy_construction()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
inline
void
PersistentObject<Derived, Connection>::notify_rhs_assignment_operation()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
inline
void
PersistentObject<Derived, Connection>::notify_lhs_assignment_operation()
{
	decrement_handle_counter();
	return;
}

template
<typename Derived, typename Connection>
inline
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
	if (m_handle_counter == 0)
	{
		throw OverflowException 
		(	"Handle counter for PersistentObject instance has reached "
			"zero and cannot be further decremented."
		);
	}
	--m_handle_counter;

	// Will not thwow, provided the destructor of Derived is non-throwing,
	// and the object is saved in the cache under m_cache_key.
	if (m_handle_counter == 0 && static_cast<bool>(m_cache_key))
	{
		m_identity_map.notify_nil_handles(*m_cache_key);
	}
	return;
}

template
<typename Derived, typename Connection>
inline
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
std::string
PersistentObject<Derived, Connection>::primary_key_name()
{
	static bool calculated_already = false;
	static std::string ret;
	if (calculated_already)
	{
		return ret;
	}
	int const primary_key_info_field = 5;
	int const column_name_field = 1;
	SharedSQLStatement statement
	(	database_connection(),
		"pragma table_info(" + Derived::primary_table_name() + ")"
	);
	int primary_keys_found = 0;
	while (statement.step())
	{	
		if (statement.extract<int>(primary_key_info_field) == 1)
		{
			if (primary_keys_found != 0)
			{
				throw DatabaseException
				(	"Multiple primary keys found when 1 expected."
				);
			}
			ret = statement.extract<std::string>(column_name_field);
			++primary_keys_found;
		}
	}
	calculated_already = true;
	return ret;
}




template
<typename Derived, typename Connection>
void
PersistentObject<Derived, Connection>::do_remove()
{
	std::string const statement_text =
		"delete from " + Derived::primary_table_name() + " where " +
		primary_key_name() + " = :p";
	SharedSQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());
	statement.step_final();
	return;
}


template
<typename Derived, typename Connection>
inline
bool
PersistentObject<Derived, Connection>::has_id() const
{
	// Relies on the fact that m_id is a boost::optional<Id>, and
	// will convert to true if and only if it has been initialized.
	return m_id;
}

template
<typename Derived, typename Connection>
inline
bool
PersistentObject<Derived, Connection>::is_orphaned() const
{
	return m_handle_counter == 0;
}

template
<typename Derived, typename Connection>
inline
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






