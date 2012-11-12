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
 * ("loaded", "loading" or "ghost").
 *
 * In the derived class, the intention is that some or all data members
 * declared in that class, can be "lazy". This means that they are not
 * initialized in the derived object's constructor, but are rather only
 * initialized at a later time via a call to load(), which in turn calls
 * the virtual method do_load (which needs to be defined in the
 * derived class).
 *
 * In the derived class, implementations of getters
 * for attributes
 * other than those that are loaded immediately on construction, should
 * have \e load() as their first statement. (This means that getters cannot
 * be const.) In addition, implementations of \e all setters in the
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
 * @todo Make save_new() and save_existing() private. Then provide a single
 * method called save() that calls save_existing() if has_id() == true,
 * otherwise calls save_new(). This will facilitate saving objects where
 * the client does not know whether the object in question has an id or
 * not (without having to expose has_id() as a public method). It will
 * also reduce the number of public methods by 1, and will remove a class
 * of exception (std::logic_error) from the interface.
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
	 * Saves the state of the in-memory object to the
	 * database, overwriting the data in the database in the
	 * event of any conflict with the existing persisted data
	 * for this id. This is done by calling pure virtual function
	 * do_save_existing, which must be defined in the derived class.
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
	 * the first statement in the implementation of any setter method in
	 * the derived class.
	 *
	 * @throws std::logic_error if this PersistentObject does not have
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
	 * of do_save_existing.
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

	/**
	 * Saves the state of the in-memory object to the database,
	 * as an additional item, rather than overwriting existing
	 * data. The id assigned to the just-saved object is then
	 * recorded in the object in memory, and can be retrieved by
	 * calling id().
	 *
	 * In the body of this function, a call is made to the pure virtual
	 * function
	 * do_save_new, which must be defined in the derived
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
	 * do_calculate_prospective_key() function in the derived class.
	 *
	 * @throws TransactionNestingException if the maximum level of transaction
	 * nesting for the DatabaseConnection has been reached (very unlikely).
	 *
	 * @throws InvalidConnection if the DatabaseConnection is invalid.
	 *
	 * @throws std::logic_error if the object already has an id, i.e. has
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


protected:

	/**
	 * Calls the derived class's implementation
	 * of do_load, if and only if the object is not already
	 * loaded. If the object does not have an id,
	 * then this function does nothing.
	 *
	 * In defining \e do_load, the derived class should throw an instance
	 * of std::exception (may be an instance of an exception class derived
	 * therefrom) in the event that the load fails. If this
	 * is adhered to, and do_load is implemented with the strong
	 * exception-safety guarantee, and do_load does not perform any
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
	 * do_load is defined:
	 *
	 * @throws TransactionNestingException in the event that the maximum
	 * level of transaction nesting for the database connection has been
	 * reached. (This is extremely unlikely.) If this occurs \e before
	 * do_load is entered, the object will be as it was before the
	 * function was called.
	 * 
	 * @throws InvalidConnection in the event that the database connection is
	 * invalid at the point the \e load function is entered. If this occurs,
	 * the object will be as it was before this function was called.
	 *
	 * Exception safety: depends on how the derived class defines \e
	 * do_load. See above.
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
	 * Note an object that is created anew, that does not already exist
	 * in the database, should not have an id. By having an id, an object
	 * is saying "I exist in the database".
	 *
	 * This method is protected and so it can be called by derived classes.
	 * However it should not be called lightly. It is intended to be called
	 * only in constructors, to assign an initial value to the id.
	 *
	 * @param p_id the value to which you want to set the id of this object.
	 *
	 * @throws std::logic_error if set_id is called on an object for which
	 * its id has already been initialized.
	 *
	 * Exception safety: <em>strong_guarantee</em>.
	 */
	void set_id(Id p_id);

	/**
	 * @returns the id that would be assigned to this instance of
	 * PersistentObject when saved to the database.
	 *
	 * This function calls \e do_calculate_prospective_key, which has a
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
	 * Exception safety: <em>strong guarantee</em>, (providing the virtual
	 * function
	 * do_get_table_name does nothing odd but simply returns a std::string
	 * as would be expected).
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
	
	/**
	 * Deliberately unimplemented. Assignment doesn't make much semantic
	 * sense for a PersistentObject that is supposed to
	 * represent a \e unique object in the database with a unique id.
	 */
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





