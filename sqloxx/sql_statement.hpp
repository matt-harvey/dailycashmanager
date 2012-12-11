#ifndef sql_statement_hpp
#define sql_statement_hpp

#include "database_connection.hpp"
#include "detail/sql_statement_impl.hpp"
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


namespace sqloxx
{


/**
 * Class to handle SQL statements that may be shared instances
 * of such statements in a cache. When a
 * SQLStatement is destructed, it is automatically
 * reset. However the underlying statement remains in
 * the cache ready for deployment (unless the cache is full).
 * The details of caching are handled within the DatabaseConnection
 * class. The client just calls the constructor and uses the statement.
 *
 * Clients of Sqloxx should not use the underlying SQLStatementImpl class
 * directly, but only ever via SQLStatement.
 *
 * If an exception is thrown by an method of SQLStatement, the
 * caller should in general no longer rely on the state of SQLStatement
 * being valid. However, when the SQLStatement goes out of scope or
 * is otherwise destroyed, the underlying SQLStatementImpl will be reset to a
 * valid state. Furthermore, a locking mechanism ensures that two
 * SQLStatements cannot share the same underlying SQLStatementImpl. This
 * prevents SQLStatementImpl instances that are in an invalid state
 * from being used unless used
 * via the very same SQLStatement that triggered the invalid state.
 */
class SQLStatement:
	private boost::noncopyable
{
public:

	/**
	 * Creates an object encapsulating a SQL statement.
	 * 
	 * @param p_database_connection is the DatabaseConnection
	 * on which the statement will be executed.
	 *
	 * @param str is the text of a single SQL statement. It can be terminated
	 * with any mixture of semicolons and/or spaces (but not other forms
	 * of whitespace).
	 *
	 * @throws InvalidConnection if p_database_connection is an
	 * invalid database connection (i.e. if p_database_connection.is_valid()
	 * returns false).
	 *
	 * @throws SQLiteException, or an exception derived therefrom, if there
	 * is some other problem in preparing the statement, which results in a
	 * SQLite error code (that is not SQLITE_OK) being returned.
	 * 
	 * @throws TooManyStatements if the first purported SQL statement
	 * in str is syntactically acceptable to SQLite, <em>but</em> there
	 * are characters in str after this statement, other than ';' and ' '.
	 * This includes the case where there are further syntactically
	 * acceptable SQL statements after the first one - as each
	 * SQLStatement can encapsulate only one statement.
	 *
	 * @throws std::bad_alloc in the very unlikely event of a memory
	 * allocation error in execution.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	SQLStatement
	(	DatabaseConnection& p_database_connection,	
		std::string const& p_statement_text
	);

	/**
	 * Destructor "clears" the state of the underlying cached
	 * SQLStatementImpl for re-use by a subsequent SQLStatement with the
	 * same statement text. (Client code does not need to concern itself
	 * with the details of this.)
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	~SQLStatement();
	
	/**
	 * Wrappers around SQLite bind functions.
	 * 
	 * @throws InvalidConnection if the database connection is invalid.
	 * If this occurs, the state of the SQLStatement will be
	 * the same as before the \e bind method was called.
	 *
	 * @throws SQLiteException, or an exception derived therefrom,
	 * if SQLite could not properly bind the statement. If this occurs,
	 * the statement will be reset and all bindings cleared.
	 *
	 * @param parameter_name named parameter embedded in
	 * the SQL statement.
	 *
	 * @param x value to be bound to the named parameter.
	 *
	 * Exception safety: <em>basic guarantee</em>.
	 */
	void bind(std::string const& parameter_name, int x);
	void bind(std::string const& parameter_name, boost::int64_t x);
	void bind(std::string const& parameter_name, double x);
	void bind(std::string const& parameter_name, std::string const& x);

	/**
	 * Where a statement has a result set available,
	 * this function (template) can be used to extract the value at
	 * the \e indexth column of the current row (where \e index starts
	 * counting at 0).
	 *
	 * Currently the following types for T are supported:\n
	 *	boost::int64_t\n
	 *	int\n
	 *	double;\n
	 *	std::string\n
	 * 
	 * @param index is the column number (starting at 0) from which to
	 * read the value.
	 * 
	 * @throws ResultIndexOutOfRange if \e index is out of range.
	 *
	 * @throws ValueTypeException if the requested column contains a type that
	 * is incompatible with T.
	 *
	 * Exception safety: <em>strong guarantee</em>.
	 */
	template <typename T>
	T extract(int index);

	/**
	 * Wraps sqlite3_step.
	 * Returns true as long as there are further steps to go (i.e. result
	 * rows to examine).
	 *
	 * @throws InvalidConnection if the database connection is invalid. If
	 * this occurs, the state of the SQLStatement will be the same as
	 * before the \e step method was called.
	 *
	 * @throws SQLiteException or some exception deriving therefrom, if an
	 * error occurs that results in a SQLite error code.
	 * This function should almost never occur, but it is
	 * possible something will fail as the statement is being executed, in
	 * which case the resulting SQLite error condition will trigger the
	 * corresponding exception class. If this occurs, the SQLStatement
	 * will be reset and all bindings cleared.
	 *
	 * Exception safety: <em>basic guarantee</em>.
	 */
	bool step();

	/**
	 * Wraps sqlite3_step. Similar to \c step except that it throws an
	 * exception if a result row still remains after calling. That is,
	 * it is equivalent to calling:\n
	 * \c if (step()) throw UnexpectedResultRow("...");\n
	 *
	 * @throws UnexpectedResultRow if a result set is returned. If this
	 * occurs, the statement is reset (but bindings are not cleared).
	 * 
	 * @throws InvalidConnection if the database connection is invalid. If
	 * this occurs, the statement is left in the same state as it was before
	 * the \e step_final method was called.
	 *
	 * @throws SQLiteException or an exception derived therefrom if there
	 * is any other error in executing the statement. If this happens, the
	 * statement will be reset and all bindings cleared.
	 *
	 * Exception safety: <em>basic guarantee</em>.
	 */
	void step_final();

	/**
	 * Resets the statement ready for subsequent
	 * re-execution - but does not clear the bound parameters.
	 * This is a wrapper for sqlite3_reset.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void reset();

	/**
	 * Clears the parameter bindings from the statement, setting all
	 * to NULL. This is a wrapper for sqlite3_clear_bindings.
	 *
	 * Exception safety: <em>nothrow guarantee</em>.
	 */
	void clear_bindings();


private:

	boost::shared_ptr<detail::SQLStatementImpl> m_sql_statement;

};


inline
SQLStatement::SQLStatement
(	DatabaseConnection& p_database_connection,
	std::string const& p_statement_text
):
	m_sql_statement
	(	DatabaseConnection::StatementAttorney::provide_sql_statement
		(	p_database_connection,
			p_statement_text
		)
	)
{
}


}  // namespace sqloxx


#endif  // sql_statement_hpp
