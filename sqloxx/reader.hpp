#ifndef GUARD_reader_hpp
#define GUARD_reader_hpp

#include "general_typedefs.hpp"
#include "sqloxx_exceptions.hpp"
#include "sql_statement.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace sqloxx
{


// READER

/**
 * Class template each instantiation of which represents a class
 * of iterator-like "reader" objects that wrap an SQL "select" statement,
 * where the field being selected is the primary key field for type T as
 * it is stored in the database. By calling the item() method of
 * a Reader, with suitable parameters, an instance of T is returned.
 * Because the Reader selects
 * from a table that is physically in the database, the implementation
 * of Reader can use a fast, unchecked function to
 * get the instance, in the knowledge that the Id is valid as it has
 * just been read from the physical database. Note, however, that
 * if an object is deleted while still being referred to by a Reader,
 * undefined behaviour may result.
 *
 * Reader classes also provide a higher-level, cleaner interface for
 * client code wishing to traverse a database table without having to
 * use SQL directly.
 * Each instantiation of Reader represents wraps a particular "select"
 * statement that is used to traverse a table or view in the database.
 * 
 * Reader deliberately does \e not conform to STL-like iterator conventions.
 * While this has the disadvantages that come with eschewing convention,
 * it does provide
 * a syntactically simple way of traversing all the persisted objects of
 * a given type, without having first to declare any kind of container
 * object.
 *
 * Thus, with a Reader, we can generally traverse all the T instances in
 * the database by doing this:
 *
 * <tt>
 * Reader<T, Connection> reader(database_connection);\n
 * while (reader.read())\n
 * {\n
 * 		reader.item().some_method_of_T();\n
 * }\n
 * </tt>
 *
 * There is no need to first declare a "Table<T>" or the like.
 *
 * Client code may use the Reader class in one of two ways:\n
 * (1) Use the Reader class as is, instantiating for a particular
 * type T and database connection type Connection. A Reader can be
 * constructed using the default constructor, or by passing a
 * different string to the p_selector parameter of the constructor;\n or
 * (2) Inherit from Reader<T, Connection>, but provide an alternative
 * constructor, that takes a Connection parameter, which it passes up to
 * the base Reader constructor, along with a custom string for p_selector
 * that is fixed for that derived class. This enables a given derived Reader
 * class to use a fixed selection statement for all instances of
 * that class.
 *
 * Parameter templates:\n
 * T should be a class derived from PersistentObject<T, Connection>
 * (see separate documentation for PersistentObject), for which
 * its primary key is of type sqloxx::Id, and which has a
 * constructor that takes a Connection& parameter,
 * an Id parameter and a char parameter (for example, an
 * instantiation of the sqloxx::Handle
 * template would meet this requirement) (see constructor
 * documentation for more details); and\n
 * Connection should be a class inheriting from DatabaseConnection,
 * for which identity_map<T, Connection>() is defined to return
 * a reference to the IdentityMap for that type T for that
 * Connection.
 */
template <typename T, typename Connection>
class Reader:
	private boost::noncopyable
{
public:

	/**
	 * Construct a Reader from a database connection of type
	 * Connection.
	 *
	 * @param p_selector SQL statement string that will select records
	 * representing instances of type T, from the database.
	 * The default string can be seen in the function signature. If a
	 * different string is provided, ti should be such that it selects
	 * only a single column, such that that column is the primary
	 * key for class T, from which instances of Handle<T> can be
	 * constructed.
	 *
	 * @param p_database_connection instance of Connection.
	 *
	 * @throws InvalidConnection if p_database_connection is an
	 * invalid database connection (i.e. if p_database_connection.is_valid()
	 * returns false).
	 *
	 * @throws SQLiteException, or an exception derived therefrom, if there
	 * is some problem in constructing the underlying SQL statement,
	 * that results
	 * in a SQLite error code being returned.
	 *
	 * @throws std::bad_alloc in the unlikely event of a memory allocation
	 * error in constructing the underlying SQL statement.
	 * 
	 * @throws TooManyStatements if the first purported SQL statement
	 * in p_selector is syntactically acceptable to SQLite, <em>but</em> there
	 * are characters in p_selector after this statement,
	 * other than ';' and ' '.
	 * This includes the case where there are further syntactically
	 * acceptable SQL statements after the first one.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	explicit Reader
	(	Connection& p_database_connection,
		std::string const& p_selector =
		(	"select " + T::primary_key_name() +
			" from " + T::primary_table_name()
		)
	);
	
	/**
	 * Advances the Reader to the next row into the result set.
	 * When the Reader
	 * is first constructed, it is notionally positioned just \e before
	 * the first result row. The first call to read() moves it "onto" the
	 * first result row (if there is one). Etc. Once it is on the final
	 * result row, calling
	 * read() again will result in it moving notionally one beyond the result
	 * row, and false will be returned.
	 * Calling read yet again will then result in the reader
	 * cycling back to the
	 * first row, and returning true again (assuming there is a first row).
	 *
	 * @returns true if, on calling read(), the Reader moves onto a result
	 * row; otherwise, returns false.
	 *
	 * @throws InvalidConnection is thrown if the database connection is
	 * invalid. If this occurs, the state of the Reader
	 * will be the same as just before the throwing call to read() was
	 * made.
	 *
	 * @throws SQLiteException or some exception deriving
	 * therefrom, if an error occurs that results in a SQLite error
	 * code. This should almost never occur. If it does occur, the
	 * Reader will be reset to the state it was in just after
	 * construction - i.e. just before it read the first result row.
	 *
	 * Exception safety: <em>basic guarantee</em>.
	 */
	bool read();
	
	/**
	 * Preconditions:\n
	 * The constructor of T should offer the strong guarantee; and\n
	 * The destructor of T should never throw; and\n
	 * T should have a constructor of the form
	 * T(Connection&, Id, char).
	 * Typically, T will be
	 * an instantiation of sqloxx::Handle - but it need not
	 * be. sqloxx::Handle uses char as a dummy parameter merely
	 * to distinguish the fast, unchecked constructor from the
	 * slow, checked constructor.
	 *
	 * @todo Do I need to document the char thing better?
	 *
	 * @returns an instance of T initialized with the constructor
	 * T(Connection&, Id, char), where the Connection passed to
	 * the Reader's constructor is passed to the Connection&
	 * parameter, and the single field being read by the
	 * Reader from the database (which should be the primary
	 * key field for type T) is passed to the Id parameter.
	 * The char field is passed the value '\0'.
	 *
	 * @throws ResultIndexOutOfRange if there is an error extracting
	 * the result of the underlying SQL statement. This would
	 * generally only occur if an invalid SQL statement was not
	 * passed to the constructor of the Reader, or if the database
	 * is corrupt.
	 *
	 * @throws ValueTypeException if the type of the column being
	 * selected by the underlying SQL statement is not compatible
	 * with the Id type required to initialize a Handle<T>. If a
	 * suitable SQL statement string is passed to the constructor
	 * of the Reader, this should never occur.
	 *
	 * @throws InvalidReader if the Reader is not currently positioned
	 * over a result row.
	 *
	 * @throws std::bad_alloc if the object (instance of T to which we
	 * want a Handle) is not already loaded in the IdentityMap (cache),
	 * and there is a memory allocation failure in the process of loading
	 * and caching the object.
	 *
	 * @throws InvalidConnection if the database connection is invalid.
	 *
	 * @throws SQLiteException, of a derivative thereof,
	 * in the event of an error during execution thrown up by the
	 * underlying SQLite API. Assuming the Reader was initialized with
	 * a valid SQL statement string, this should almost never occur.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	T item() const;

private:
	Connection& m_database_connection;
	SQLStatement mutable m_statement;
	bool m_is_valid;

};  // Reader


template <typename T, typename Connection>
Reader<T, Connection>::Reader
(	Connection& p_database_connection,
	std::string const& p_selector
):
	m_database_connection(p_database_connection),
	m_statement(p_database_connection, p_selector),
	m_is_valid(false)
{
}

template <typename T, typename Connection>
inline
bool
Reader<T, Connection>::read()
{
		try
		{
			// Assignment is intentional
			return m_is_valid = m_statement.step();
		}
		catch (SQLiteException&)
		{
			m_is_valid = false;
			throw;
		}
}

template <typename T, typename Connection>
T
Reader<T, Connection>::item() const
{
	if (m_is_valid)
	{
		return T(m_database_connection, m_statement.extract<Id>(0), '\0');
	}
	assert (!m_is_valid);
	throw InvalidReader("Reader is not a result row.");
}

	

}  // namespace sqloxx


#endif  // GUARD_reader_hpp
