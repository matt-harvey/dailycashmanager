#ifndef GUARD_persistent_object_hpp
#define GUARD_persistent_object_hpp

#include "database_connection.hpp"
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <exception>
#include <stdexcept>
#include <string>



namespace sqloxx
{

/**
 * Class for creating objects persisted to a database. This
 * should be inherited by a derived class and the pure virtual
 * functions (and possibly non-pure virtual functions) provided with
 * definitions (or possibly redefinitions in the case of the non-pure
 * virtual functions). The class provides for lazy loading behaviour,
 * using the "ghost" pattern as described on p. 202 of Martin Fowler's
 * "Patterns of Enterprise Application Architecture". The PersistentObject
 * base class provides the bookkeeping associated with this pattern,
 * keeping track of the loading status of each in-memory object
 * ("loaded", "loading" or "ghost"). Derived classes are responsible for
 * specifying their instances are loaded and saved.
 *
 *
 * @todo Provide for atomic saving (not just of
 * SQL execution, but of the actual alteration of the in-memory objects).
 * Go through all the client classes in Phatbooks and ensure the
 * do_save... functions in each are atomic with respect to
 * the in-memory objects, and conform to the restrictions detailed in the
 * PersistentObject API documentation. (Note I have already done this
 * for \e load functions.)
 *
 * @todo All the boost::optional<T> data members in the derived classes
 * of PersistentObject in phatbooks, are not actually required to be
 * boost::optional<T>. Given that load() is called before any of these
 * are retrieved, the should never be accessed in an uninitialized state
 * anyway; and when the object is first constructed, they can be initialized
 * to any arbitrary value. The m_loading_status member of PersistentObject
 * keeps track of whether loading is required. The only point of the wrapping
 * in optional is to guard against a programming error whereby I forget to
 * insert a call to load() in a getter. The use of boost::optional means
 * that if this happens, the program will fail loudly rather than silently.
 * On the other hand, the use of boost::optional adds overhead and complexity,
 * and makes the code harder to read. Decide on a policy re. whether to use
 * boost::optional, and stick to it.
 *
 * @todo Unit testing.
 */
class PersistentObject
{
public:

	typedef int Id;

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
	 * Destructor.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	virtual ~PersistentObject();

	/**
	 * Calls the derived class's implementation
	 * of do_load_all, if the object is not already
	 * fully loaded. If the object does not have an id,
	 * then this function does nothing.
	 *
	 * In defining \e do_load_all, the derived class should throw an instance
	 * of std::exception (may be an instance of an exception class derived
	 * therefrom) in the event that the load fails. If this
	 * is adhered to, and do_load_all is implemented with the strong
	 * exception-safety guarantee, and do_load_all does not perform any
	 * write operations on the database, or have other side-effects, then the
	 * \e load function will itself provide the strong exception safety
	 * guarantee.
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
	 * data. The id assigned to the just-saved object is then
	 * recorded in the object in memory, and can be retrieved by
	 * calling id().
	 *
	 * In the body of this function, a call is made to the pure virtual
	 * function
	 * do_save_new_all, which must be defined in the derived
	 * class. (But note that the base class save_new takes care of assigning
	 * the id and also wraps the save operation as a SQL transaction by
	 * calling the begin_transaction and end_transaction methods of the
	 * DatabaseConnection.)
	 *
	 * The do_get_table_name function must also
	 * be defined in the derived class in order for this function
	 * to find an automatically generated id to assign to the object
	 * when saved. By default it is assumed that the id is an auto-
	 * incrementing integer primary key generated by SQLite. However this
	 * behaviour can be overridden by redefining the
	 * do_calculate_prospective_key function in the derived class.
	 *
	 * Exception safety: depends on the exception safety of
	 * \e do_save_new_all. Providing both do_save_new_all offers the strong
	 * guarantee, then so does \e save_new.
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
	 * Exception safety: <em>nothrow guarantee</em>.
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
	 * Should not be called by any functions other than prospective_key.
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
	 * Exception safety: <em>strong guarantee</em>
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


private:

	/**
	 * Clears the loading status back to \e ghost.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void clear_loading_status();
	
	// Deliberately unimplemented. Assignment doesn't make much semantic
	// sense for a PersistentObject that is supposed to
	// represent a \e unique object in the database with a unique id.
	PersistentObject& operator=(PersistentObject const& rhs);

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




}  // namespace sqloxx



#endif  // GUARD_persistent_object_hpp





