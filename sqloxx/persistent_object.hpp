#ifndef GUARD_persistent_object_hpp
#define GUARD_persistent_object_hpp

#include "general_typedefs.hpp"
#include "identity_map.hpp"
#include "next_auto_key.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/none.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <jewel/optional.hpp>
#include <exception>


namespace sqloxx
{


// WARNING The whole thing needs the documentation revised if I proceed with
// the CRTP pattern

/**
 * Class template for creating objects persisted to a database. This
 * should be inherited by a derived class and the pure virtual
 * functions (and possibly non-pure virtual functions) provided with
 * definitions (or possibly redefinitions in the case of the non-pure
 * virtual functions).
 *
 * An instance of (a class deriving from an instantiation of)
 * PersistentObject represents a "business object" for which the
 * data will be stored in a relational database (currently only
 * SQLite is supported) under a single primary key.
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
 * In addition, the following function is provided with a default
 * implementation by PersistentObject, but may be redefined in the
 * derived class:
 *
 * <b>virtual Id do_calculate_prospecitve_key() const;</b>\n
 * See documentation for prospective_key() function.
 *
 * TEMPLATE PARAMETERS
 *
 * @param Derived The derived class. Derived should inherit publicly
 * from PersistentObject per the Curiously Recurring Template Pattern (CRTP),
 * thus: <tt>class Derived: public PersistentObject<Derived...>, where "..."
 * represents the other template parameters, which may be ommitted if
 * the default values are accepted.
 *
 * @param Connection The type of the database connection through which
 * instances of Derived will be persisted to the database. Connection
 * should be a class derived from sqloxx::DatabaseConnection.
 *
 * @param Id The type of the identifier for this class in the database.
 * Best if integral. Defaults to sqloxx::Id.
 *
 * @param HandleCounter The type of the counter to count the number of
 * "at large"
 * "handles" to a given Derived object. Should be an integral type,
 * and defaults to int. (The count of handles is used by IdentityMap class
 * to help manage caching of in-memory objects.)
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
 *
 * @todo I need a better way of conveying the Id type to derived classes.
 * Currently this information is "multiply located".
 */
template
<	typename Derived,     // subclass of PersistentObject
	typename Connection,  // subclass of DatabaseConnection for this app.
	typename Id = sqloxx::Id,  // type of primary key for Derived
	typename HandleCounter = int     // type with which we will be counting handles
>
class PersistentObject
{
public:

	typedef sqloxx::IdentityMap<Derived, Connection> IdentityMap;

	/**
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
	 * If the object has an id, this calls save_existing(). Otherwise, it
	 * calls save_new().
	 *
	 * For details of exceptions and exception safety, see the documentation
	 * for save_existing() and save_new(). But note that unlike those two
	 * functions, it is impossible for save() to throw sqloxx::LogicError.
	 */
	void save();

	/**
	 * Saves the state of the in-memory object to the
	 * database, overwriting the data in the database in the
	 * event of any conflict with the existing persisted data
	 * for this id. This is done by calling pure virtual function
	 * do_save_existing(), which must be defined in the derived class.
	 *
	 * Note the implementation is wrapped as a transaction
	 * by calls to the begin_transaction and end_transaction
	 * methods of the DatabaseConnection. This wrapping is taken
	 * care of by the base save_existing() method.
	 *
	 * Note there is a call to load() within the base method, prior
	 * to entering the save transaction, to ensure the object is
	 * not saved in a partial state. This call to load will only have
	 * effect if the object is in a partial or "ghost" state when it
	 * is called. The upshot of this is that, in order to make sure that
	 * changes to the in-memory object remain in the in-memory object and
	 * are subsequently written to the database
	 * when save_existing() is called, you should always call load() as
	 * the \e first statement in the implementation of any \e setter method in
	 * the derived class.
	 *
	 * @throws sqloxx::LogicError if this PersistentObject does not have
	 * an id. 
	 *
	 * @throws TransactionNestingException if the maximum transaction
	 * nesting level of the DatabaseConnection has been reached (very
	 * unlikely).
	 *
	 * @throws InvalidConnection if the DatabaseConnection is
	 * invalid.
	 *
	 * Other exceptions that may be thrown depend on the derived
	 * class's implementation
	 * of do_save_existing().
	 *
	 * Exception safety: depends on the derived class's implementation
	 * of do_save_existing(). In implementing this method, the derived
	 * class should not make any assumptions about whether the final call
	 * to end_transaction() (made in the base method after do_save_existing()
	 * has exited) succeeds or fails. If do_save_existing() is implemented
	 * in this way, and also offers the basic guarantee, then save_existing()
	 * will itself offer the <em>basic guarantee</em>.
	 */
	void save_existing();

	// WARNING Up to here is reassessing exception safety, documentation
	// and testing in light of IdentityMap pattern.

	/**
	 * Saves the state of the in-memory object to the database,
	 * as an additional item, rather than overwriting existing
	 * data. The id assigned to the just-saved object is then
	 * recorded in the object in memory, and can be retrieved by
	 * calling id().
	 *
	 * In the body of this function, a call is made to the pure virtual
	 * function
	 * do_save_new(), which must be defined in the derived
	 * class. (But note that the base class save_new takes care of assigning
	 * the id and also wraps the call to do_save_new as a SQL transaction by
	 * calling the begin_transaction and end_transaction methods of the
	 * Connection.)
	 *
	 * The do_save_new function should result in the
	 * saving of the complete state of a complete object. After do_save_new
	 * is called, the body of the base save_new function will mark the object
	 * as being in a loaded, i.e. complete state.
	 *
	 * The primary_table_name() function must also
	 * be defined in the derived class in order for this function
	 * to find an automatically generated id to assign to the object
	 * when saved. By default it is assumed that the id is an auto-
	 * incrementing integer primary key generated by SQLite. However this
	 * behaviour can be overridden by redefining the
	 * do_calculate_prospective_key() function in the derived class.
	 *
	 * @throws TransactionNestingException if the maximum level of transaction
	 * nesting for the DatabaseConnection has been reached (very unlikely).
	 *
	 * @throws InvalidConnection if the DatabaseConnection is invalid.
	 *
	 * @throws sqloxx::LogicError if the object already has an id, i.e. has
	 * already been saved to the database.
	 *
	 * May also throw exceptions from do_calculate_prospective_key(), which
	 * is invoked in the body of this function. See documentation
	 * for do_calculate_prospective_key for exceptions that might be thrown
	 * by the default version of that function.
	 *
	 * Exception safety: depends on how
	 * do_calculate_prospective_key() and
	 * do_save_new() are implemented. Providing that neither of these
	 * functions
	 * affects the state of the in-memory object,
	 * then save_new() provides the <em>strong guarantee</em>.
	 */
	void save_new();

	/**
	 * @returns the id of the object, if it has one.
	 *
	 * @throws jewel::UninitializedOptionalException if the object doesn't
	 * have an id.
	 */
	Id id() const;
	
	/**
	 * Should only be called by IdentityMap<Derived>.
	 *
	 * WARNING This should be able to be specified in the constructor. But we
	 * also don't want to confuse it with the other
	 * two-paramatered constructor!
	 *
	 * @todo Document and test.
	 */
	void set_proxy_key(Id p_proxy_key);

	/**
	 * Should only be called by Handle<Derived>. To advise the underlying
	 * object that a handle pointing to it has been constructed (not copy-
	 * constructed, but ordinarily constructed).
	 * 
	 * @todo Document and test.
	 */
	void notify_handle_construction();

	/**
	 * Should only be called by Handle<Derived>. To advise the underlying
	 * object that a handle pointing to it has been copy-constructed.
	 * 
	 * @todo Document and test.
	 */
	void notify_handle_copy_construction();

	/**
	 * Should only be called by Handle<Derived>. To advise the
	 * underlying object that a handle pointing to it has appeared
	 * as the right-hand operand of an assignment operation.
	 *
	 * @todo Document and test.
	 */
	void notify_rhs_assignment_operation();
	
	/**
	 * Should only be called by Handle<Derived>. To advise the
	 * underlying object that a handle pointing to it has appeared
	 * as the right-hand operand of an assignment operation.
	 *
	 * @todo Document and test.
	 */
	void notify_lhs_assignment_operation();

	/**
	 * Should only be called by Handle<Derived>. To advise the underlying
	 * object that a handle pointing to it has been destructed.
	 *
	 * @todo Document and test.
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
	 */
	bool is_orphaned() const;

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
	 * @todo Determine exception safety.
	 */
	Connection& database_connection() const;


	/**
	 * @returns the id that would be assigned to this instance of
	 * PersistentObject when saved to the database.
	 *
	 * This function calls \e do_calculate_prospective_key, which has a
	 * default implementation but may be redefined.
	 *
	 * @throws sqloxx::LogicError in the event this instance already has
	 * an id. (This occurs regardless of how/whether
	 * \e do_calculate_prospective_key is redefined.)
	 *
	 * Apart from \e sqloxx::LogicError as just described, the exception
	 * throwing behaviour and exception safety of this function depend on
	 * those of the function do_calculate_prospective_key().
	 *
	 * <b>If the default implementation of do_calculate_prospective_key() is
	 * retained, then the following exceptions may be thrown, in addition
	 * to sqloxx::LogicError.</b>
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

	/**
	 * Provides implementation for the public function prospective_key.
	 */
	virtual Id do_calculate_prospective_key() const;

	/**
	 * See documentation for \e load function.
	 *
	 * Exception safety: <em>depends on function definition
	 * provided by derived class</em>
	 */
	virtual void do_load() = 0;

	/**
	 * See documentation for public <em>save_existing</em> function.
	 *
	 * Exception safety: <em>depends on function definition provided by
	 * derived class</em>.
	 */
	virtual void do_save_existing() = 0;

	/**
	 * See documentation for public <em>save_new</em> function.
	 *
	 * Exception safety: <em>depends on function definition provided by
	 * derived class</em>.
	 */
	virtual void do_save_new() = 0;

	/**
	 * Clears the loading status back to \e ghost.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void clear_loading_status();
	
	/**
	 * Deliberately unimplemented. Assignment doesn't make much semantic
	 * sense for a PersistentObject that is supposed to
	 * represent a \e unique object in the database with a unique id.
	 */
	PersistentObject& operator=(PersistentObject const& rhs);
	
	void increment_handle_counter();
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
	// IdentityMap<Derived> can look up a PersistentObject either via its Id
	// (which corresponds to its primary key in the database), or via
	// it proxy_key. PersistentObject instances that are newly created and
	// have not yet been saved to the database will not have an id (i.e. m_id
	// will be in an uninitialized state), however these may still be managed
	// by the IdentityMap, and so still need a means for the IdentityMap to
	// identify them in their internal cache.
	boost::optional<Id> m_proxy_key;

	LoadingStatus m_loading_status;
	HandleCounter m_handle_counter;
};


template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
PersistentObject<Derived, Connection, Id, HandleCounter>::PersistentObject
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
<typename Derived, typename Connection, typename Id, typename HandleCounter>
PersistentObject<Derived, Connection, Id, HandleCounter>::PersistentObject
(	IdentityMap& p_identity_map	
):
	m_identity_map(p_identity_map),
	m_loading_status(ghost),
	m_handle_counter(0)
	// Note m_proxy_key is left unitialized. It is the responsibility
	// of IdentityMap<Derived> to call set_proxy_key after construction,
	// before providing a Handle to a newly created Derived instance.
{
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
PersistentObject<Derived, Connection, Id, HandleCounter>::~PersistentObject()
{
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::load()
{
	while (m_loading_status == loading)
	{
		// Wait
	}
	if (m_loading_status == ghost && has_id())
	{
		m_loading_status = loading;
		try
		{
			database_connection().begin_transaction();
		}
		catch (TransactionNestingException&)
		{
			clear_loading_status();
			throw;
		}
		catch (InvalidConnection&)
		{
			clear_loading_status();
			throw;
		}
		try
		{
			do_load();
		}
		catch (std::exception&)
		{
			clear_loading_status();
			throw;
		}
		try
		{
			database_connection().end_transaction();
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
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::save()
{
	if (has_id())
	{
		save_existing();
	}
	else
	{
		save_new();
	}
	return;
}


template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::save_existing()
{
	if (!has_id())
	{
		throw LogicError
		(	"Method save_existing() called on an instance of PersistentObject"
			" that does not correspond with an existing database record."
		);
	}
	load();
	database_connection().begin_transaction();
	do_save_existing();
	database_connection().end_transaction();
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
Id
PersistentObject<Derived, Connection, Id, HandleCounter>::
prospective_key() const
{
	if (has_id())
	{
		throw LogicError
		(	"Object already has id so prospective_key does not apply."
		);
	}
	return do_calculate_prospective_key();
}


template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
Id
PersistentObject<Derived, Connection, Id, HandleCounter>::
do_calculate_prospective_key() const
{	
	return next_auto_key<Connection, Id>
	(	database_connection(),
		Derived::primary_table_name()
	);
}


template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::save_new()
{
	Id const allocated_id = prospective_key();  // throws if has_id()
	assert (!has_id());
	database_connection().begin_transaction();
	do_save_new();
	database_connection().end_transaction();
	m_id = allocated_id;
	if (m_proxy_key)
	{
		m_identity_map.register_id(*m_proxy_key, allocated_id);
	}

	// The next line fixed a bug 2012-11-22 that in resulted in objects being
	// re-loaded from database when they were already complete.
	m_loading_status = loaded;

	return;
	// WARNING Reconsider what exception safety guarantee can be offered
	// in light of the dealings around proxy key that have now been
	// incorporated into this function. Reflect in API documentation.
}


template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
Id
PersistentObject<Derived, Connection, Id, HandleCounter>::id() const
{
	return jewel::value(m_id);
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
set_proxy_key(Id p_proxy_key)
{
	m_proxy_key = p_proxy_key;
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
notify_handle_construction()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
notify_handle_copy_construction()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
notify_rhs_assignment_operation()
{
	increment_handle_counter();
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
notify_lhs_assignment_operation()
{
	decrement_handle_counter();
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
notify_handle_destruction()
{
	decrement_handle_counter();
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
increment_handle_counter()
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
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
decrement_handle_counter()
{
	if (m_handle_counter == 0)
	{
		throw OverflowException 
		(	"Handle counter for PersistentObject instance has reached "
			"zero and cannot be further decremented."
		);
	}
	--m_handle_counter;
	if (m_handle_counter == 0 && static_cast<bool>(m_proxy_key))
	{
		m_identity_map.notify_nil_handles(*m_proxy_key);
	}
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
Connection&
PersistentObject<Derived, Connection, Id, HandleCounter>::
database_connection() const
{
	return m_identity_map.connection();
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
bool
PersistentObject<Derived, Connection, Id, HandleCounter>::has_id() const
{
	// Relies on the fact that m_id is a boost::optional<Id>, and
	// will convert to true if and only if it has been initialized.
	return m_id;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
inline
bool
PersistentObject<Derived, Connection, Id, HandleCounter>::is_orphaned() const
{
	return m_handle_counter == 0;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::
clear_loading_status()
{
	m_loading_status = ghost;
	return;
}

template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
PersistentObject<Derived, Connection, Id, HandleCounter>::PersistentObject
(	PersistentObject const& rhs
):
	m_identity_map(rhs.m_identity_map),
	m_id(rhs.m_id),
	m_proxy_key(rhs.m_proxy_key),
	m_loading_status(rhs.m_loading_status)
{
}
		
template
<typename Derived, typename Connection, typename Id, typename HandleCounter>
void
PersistentObject<Derived, Connection, Id, HandleCounter>::swap_base_internals
(	PersistentObject& rhs
)
{
	IdentityMap temp_id_map = rhs.m_identity_map;
	boost::optional<Id> temp_id = rhs.m_id;
	boost::optional<Id> temp_proxy_key = rhs.m_proxy_key;
	LoadingStatus temp_loading_status = rhs.m_loading_status;

	rhs.m_identity_map = m_identity_map;
	rhs.m_id = m_id;
	rhs.m_proxy_key = m_proxy_key;
	rhs.m_loading_status = m_loading_status;

	m_identity_map = temp_id_map;
	m_id = temp_id;
	m_proxy_key = temp_proxy_key;
	m_loading_status = temp_loading_status;

	return;
}



}  // namespace sqloxx



#endif  // GUARD_persistent_object_hpp






