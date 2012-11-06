#ifndef GUARD_persistent_object_hpp
#define GUARD_persistent_object_hpp

#include "database_connection.hpp"
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <string>



namespace sqloxx
{

/**
 * Class template for creating objects persisted to a database.
 *
 * @todo Provide for atomicity of loading and saving (not just of
 * SQL execution, but of the actual alteration of the in-memory objects).
 * Go through all the client classes in phatbooks and ensure the
 * do_load... and do_save... functions in each are atomic with respect to
 * the in-memory objects, and conform to the restrictions detailed in the
 * PersistentObject API documentation.
 *
 * @todo Unit testing.
 */
template <typename Id>
class PersistentObject
{
public:

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
	 * Exception safety: <em>nothrow guarantee</em> (though derived classes'
	 * constructors might, of course, throw).
	 */
	PersistentObject
	(	boost::shared_ptr<DatabaseConnection> p_database_connection,
		Id p_id
	);

	/** 
	 * Create a PersistentObject that does \e not correspond to
	 * one that already exists in the database.
	 *
	 * @param p_database_connection database connection with which the
	 * PersistentObject is to be associated.
	 *
	 * Exception safety: <em>nothrow guarantee</em> (though derived classes'
	 * constructors might, of course, throw).
	 */
	explicit
	PersistentObject
	(	boost::shared_ptr<DatabaseConnection> p_database_connection
	);

	/**
	 * Exception safety: <em>nothrow guarantee</em> (though this destructor
	 * cannot, of course, offer any guarantees about the exception safety
	 * of derived classes' destructors).
	 */
	virtual ~PersistentObject();

	/**
	 * Calls the derived class's implementation
	 * of do_load_all, if the object is not already
	 * fully loaded. If the object does not have an id,
	 * then this function does nothing.
	 *
	 * In defining \e do_load_all, the derived class should call
	 * clear_loading_status in the event that the load fails. If this
	 * is adhered to, and do_load_all is implemented with the strong
	 * exception-safety guarantee, and do_load_all does not perform any
	 * read operations on the database, then the \e load function will itself
	 * provide the strong exception safety guarantee.
	 *
	 * Note the implementation is wrapped as a transaction
	 * by calls to begin_transaction and end_transaction
	 * methods of the DatabaseConnection.
	 *
	 * The following exceptions may be thrown regardless of how
	 * do_load_all is defined:
	 *
	 * @throws TransactionNestingException in the event that the maximum
	 * level of transaction nesting for the database connection has been
	 * reached. (This is extremely unlikely.) If this occurs \e before
	 * do_load_all is entered, the object will be as it was before the
	 * function was called.
	 * 
	 * @throws InvalidConnection in the event that the database connection is
	 * invalid at the point the \e load function is entered. If this occurs,
	 * the object will be as it was before this function was called.
	 *
	 * Exception safety: depends on how the derived class defines \e
	 * do_load_all. See above.
	 */
	void load();

	/**
	 * Saves the state of the in-memory object to the
	 * database, overwriting the data in the database in the
	 * event of any conflict with the existing persisted data
	 * for this id. This is done by calling
	 * do_save_existing_partial (in the event the object is not
	 * fully loaded) or do_save_existing_all (in the event the object
	 * is fully loaded). The do_save_... functions should be defined in
	 * the derived class.
	 *
	 * Note the implementation is wrapped as a transaction
	 * by calls to begin_transaction and end_transaction
	 * methods of the DatabaseConnection.
	 *
	 * Exception safety: depends on the exception safety of
	 * do_save_existing_all and do_save_existing_partial. If these
	 * functions provide the strong guarantee, then so does
	 * \e save_existing.
	 */
	void save_existing();

	/**
	 * Saves the state of the in-memory object to the database,
	 * as an additional item, rather than overwriting existing
	 * data. This is done by calling the pure virtual function
	 * do_save_new_all, which must be defined in the derived
	 * class. Note the do_get_table_name function must also
	 * be defined in the derived class in order for this function
	 * to find an automatically generated id to assign to the object
	 * when saved. By default it is assumed that the id is an auto-
	 * incrementing integer primary key generated by SQLite. However this
	 * behaviour can be overridden by redefining the
	 * do_calculate_prospective_key function in the derived class.
	 *
	 * Note the implementation is wrapped as a transaction by call to
	 * begin_transaction and end_transaction methods of
	 * DatabaseConnection.
	 *
	 * Exception safety: depends on the exception safety of
	 * \e do_save_new_all. Providing both (a) the \e Id type is
	 * a built in type (the constructors of which can't throw) and
	 * (b) do_save_new_all offers the strong guarantee, then \e
	 * save_new also offers the strong guarantee.
	 */
	void save_new();

	/**
	 * @returns the id of the object, if it has one.
	 *
	 * @throws jewel::UninitializedOptionalException if the object doesn't
	 * have an id.
	 */
	Id id() const;

protected:

	/**
	 * @returns a boost::shared_ptr to the database connection with which
	 * this instance of PersistentObject is associated. This is where the
	 * object will be loaded from or saved to, as the case may be.
	 *
	 * Exception safety: <em>nothrow guarantee</em>
	 */
	boost::shared_ptr<DatabaseConnection> database_connection() const;

	/**
	 * Sets the id of this instance of PersistentObject to p_id.
	 *
	 * @param p_id the value to which you want to set the id of this object.
	 *
	 * Note an object that is created anew, that does not already exist
	 * in the database, should not have an id. By having an id, an object
	 * is saying "I exist in the database".
	 *
	 * Exception safety: <em>nothrow guarantee</em>, <em>providing</em> Id is
	 * a built-in type. If it's not, then no guarantee is offered, as
	 * std::bad_alloc might be thrown.
	 */
	void set_id(Id p_id);

	/**
	 * @returns the id that would be assigned to the this instance of
	 * PersistentObject when saved to the database.
	 *
	 * This function calls /e do_calculate_prospective_key, which has a
	 * default implementation but may be redefined.
	 *
	 * @throws std::logic_error in the event this instance already has
	 * an id. (This occurs regardless of how/whether
	 * \e do_calculate_prospective_key is redefined.)
	 *
	 * Apart from \e std::logic_error as just described, the exception
	 * throwing behaviour and exception safety of this function depend on
	 * those of the function PersistentObject::do_calculate_prospective_key.
	 */
	Id prospective_key() const;

	/**
	 * @returns \e true if this instance of PersistentObject has
	 * an valid id; otherwise returns \e false.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	bool has_id() const;

	/**
	 * <em>The documentation for this function refers only to the
	 * default definition provided by PersistentObject.</em>
	 *
	 * Provides an implementation for the public function prospective_key.
	 * Should not be called by any functions are than prospective_key.
	 *
	 * @throws sqloxx::TableSizeException if the greatest primary key value
	 * already in the table (i.e. the table into which this instance of
	 * PersistentObject would be persisted) is the maximum value for the
	 * type \e Id, so that another row could not be inserted without overflow.
	 *
	 * @throws sqloxx::DatabaseException, or a derivative therefrom, may
	 * be thrown if there is some other
	 * error finding the next primary key value. This should not occur except
	 * in the case of a corrupt database, or a memory allocation error
	 * (extremely unlikely), or the database connection being invalid
	 * (including because not yet connected to a database file).
	 * @throws sqloxx::InvalidConnection if the database connection
	 * associated with this instance of PersistentObject is invalid.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	virtual Id do_calculate_prospective_key() const;

	/**
	 * See documentation for public \e load function.
	 *
	 * Exception safety: <em>depends on function definition
	 * provided by derived class</em>
	 */
	virtual void do_load_all() = 0;

	/**
	 * See documentation for public <em>save_existing</em> function.
	 *
	 * Exception safety: <em>depends on function definition provided by
	 * derived class</em>.
	 */
	virtual void do_save_existing_all() = 0;

	/**
	 * See documentation for public <em>save_existing</em> function.
	 *
	 * Exception safety: <em>depends on function definition provided by
	 * derived class</em>.
	 */
	virtual void do_save_existing_partial() = 0;

	/**
	 * See documentation for public <em>save_new</em> function.
	 *
	 * Exception safety: <em>depends on function definition provided by
	 * derived class</em>.
	 */
	virtual void do_save_new_all() = 0;

	/**
	 * This function should be defined in the derived class to return the
	 * name of the table in which instances of the derived class are stored
	 * in the database. This function is in turn called by the default
	 * implementation of \e do_calculate_prospective_key, which is in turn
	 * called by \e save_new.
	 *
	 * Exception safety: <em>depends on function definition provided by
	 * derived class</em>.
	 */
	virtual std::string do_get_table_name() const = 0;

	/**
	 * Clears the loading status back to \e ghost.
	 *
	 * Should be called by do_load_all if the load fails.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void clear_loading_status();


private:

	enum LoadingStatus
	{
		ghost = 0,
		loading,
		loaded
	};

	// Data members

	boost::shared_ptr<DatabaseConnection> m_database_connection;
	boost::optional<Id> m_id;
	LoadingStatus m_loading_status;
};





template <typename Id>
inline
PersistentObject<Id>::PersistentObject
(	boost::shared_ptr<DatabaseConnection> p_database_connection,
	Id p_id
):
	m_database_connection(p_database_connection),
	m_id(p_id),
	m_loading_status(ghost)
{
}


template <typename Id>
inline
PersistentObject<Id>::PersistentObject
(	boost::shared_ptr<DatabaseConnection> p_database_connection
):
	m_database_connection(p_database_connection),
	m_loading_status(ghost)
{
}


template <typename Id>
inline
PersistentObject<Id>::~PersistentObject()
{
}


template <typename Id>
void
PersistentObject<Id>::load()
{
	if (m_loading_status == ghost && has_id())
	{
		m_loading_status = loading;
		try
		{
			m_database_connection->begin_transaction();
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
		do_load_all();
		try
		{
			m_database_connection->end_transaction();
			// Note this can't possibly throw TransactionNestingException
			// here, unless do_load_all() has done something perverse.
		}
		catch (InvalidConnection&)
		{
			// As do_load_all has already completed, the object in
			// memory should be non-corrupt and fully loaded. The fact that
			// the database connection is now invalid only affects the
			// database, not the in-memory object. The invalidity of the
			// database connection will presumably be detected and dealt with
			// the next time it is accessed. We therefore do \e not rethrow
			// here.
			//
			// WARNING Am I really comfortable with this?
		}
		m_loading_status = loaded;
	}
	return;
}


template <typename Id>
void
PersistentObject<Id>::save_existing()
{
	start:
	switch (m_loading_status)
	{
	case loaded:
		m_database_connection->begin_transaction();
		do_save_existing_all();
		m_database_connection->end_transaction();
		break;
	case ghost:
		m_database_connection->begin_transaction();
		do_save_existing_partial();
		m_database_connection->end_transaction();
		break;
	case loading:
		goto start;
		break;
	default:
		throw std::logic_error("Loading status not recognized.");
	}
	return;
}


template <typename Id>
Id
PersistentObject<Id>::prospective_key() const
{
	if (has_id())
	{
		throw std::logic_error
		(	"Object already has id so prospective_key does not apply."
		);
	}
	return do_calculate_prospective_key();
}


template <typename Id>
inline
Id
PersistentObject<Id>::do_calculate_prospective_key() const
{	
	return database_connection()->template next_auto_key<Id>
	(	do_get_table_name()
	);
}


template <typename Id>
inline
void
PersistentObject<Id>::save_new()
{
	m_database_connection->begin_transaction();
	Id const key = prospective_key();
	do_save_new_all();
	m_database_connection->end_transaction();
	set_id(key);
	return;
}


template <typename Id>
inline
boost::shared_ptr<DatabaseConnection>
PersistentObject<Id>::database_connection() const
{
	return m_database_connection;
}


template <typename Id>
inline
Id
PersistentObject<Id>::id() const
{
	return jewel::value(m_id);
}

template <typename Id>
inline
void
PersistentObject<Id>::set_id(Id p_id)
{
	m_id = p_id;
	return;
}


template <typename Id>
inline
bool
PersistentObject<Id>::has_id() const
{
	// Relies on the fact that m_id is a boost::optional<Id>, and
	// will convert to true if and only if it has been initialized.
	return m_id;
}


template <typename Id>
inline
void
PersistentObject<Id>::clear_loading_status()
{
	m_loading_status = ghost;
	return;
}


}  // namespace sqloxx

#endif  // GUARD_persistent_object





