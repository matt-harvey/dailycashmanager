#ifndef shared_sql_statement_hpp
#define shared_sql_statement_hpp

#include "sqloxx/detail/sql_statement.hpp"
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


namespace sqloxx
{

class DatabaseConnection;
class SQLStatementManager;


/**
 * Class to handle SQL statements that may be shared instances
 * of such statements in a cache. When a
 * SharedSQLStatement is destructed, it is automatically
 * reset. However the underlying statement remains in
 * the cache ready for deployment (unless the cache is full).
 * The details of caching are handled within the DatabaseConnection
 * class. The client just calls the constructor and uses the statement.
 *
 * @todo HIGH PRIORITY In functions that delegate to the underlying
 * SQLStatement, there
 * are circumstances in which the underlying SQLStatement method may
 * throw. If this happens, the state of the SharedSQLStatement - and, for
 * that matter, the underlying SQLStatement - needs to
 * be valid afterwards, not left in some intermediate state that might
 * cause trouble if reused later. This is particularly important given
 * the opportunities for reuse afforded by the cached nature of the
 * SharedSQLStatement implementation. On determining this reaction of
 * state to exceptions, the reaction needs to be documented and tested.
 */
class SharedSQLStatement
{
public:
	
	/**
	 * Creates an object encapsulating a SQL statement.
	 * 
	 * @param p_database_connection is the DatabaseConnection
	 * on which the statement will be executed.
	 *
	 * @param p_statement_text is text of the statement.
	 *
	 * @throws InvalidConnection if an p_database_connection is an
	 * invalid database connection (i.e. if p_database_connection.is_valid()
	 * returns false).
	 *
	 * @throws SQLiteException, or an exception derived therefore, if there
	 * is some other problem in preparing the statement, which results in a
	 * SQLite error code (that is not SQLITE_OK) being returned.
	 */
	SharedSQLStatement
	(	DatabaseConnection& p_database_connection,	
		std::string const& p_statement_text
	);

	~SharedSQLStatement();

	/**
	 * Wrappers around SQLite bind functions.
	 *
	 * These throw \c SQLiteException, or an exception derived therefrom,
	 * if SQLite could not properly bind the statement.
	 */
	void bind(std::string const& parameter_name, int value);
	void bind(std::string const& parameter_name, boost::int64_t value);
	void bind(std::string const& parameter_name, std::string const& str);


	/**
	 * Where a SQLStatement has a result set available,
	 * this function (template) can be used to extract the value at
	 * the \c indexth column of the current row (where \c index starts
	 * counting at 0).
	 *
	 * Currently the following types for T are supported:\n
	 *	\c boost::int64_t\n
	 *	int\n
	 *	double\n
	 *	std::string\n
	 * 
	 * @param index is the column number (starting at 0) from which to
	 * read the value.
	 * 
	 * @throws ResultIndexOutOfRange if \c index is out of range.
	 *
	 * @throws ValueTypeException if the requested column contains a type that
	 * is incompatible with T.
	 */
	template <typename T>
	T extract(int index);

	/**
	 * Wraps sqlite3_step.
	 * Returns true as long as there are further steps to go (i.e. result
	 * rows to examine).
	 *
	 * @throws SQLiteException or some exception deriving therefrom, if an
	 * error occurs. This function should almost never throw, but it is
	 * possible something will fail as the statement is being executed, in
	 * which the resulting SQLite error condition will trigger the
	 * corresponding exception class.
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
	 * @throws SQLiteException or an exception derived therefrom if there
	 * is any other error in executing the statement.
	 */
	void step_final();

	/**
	 * Resets the statement ready for subsequent
	 * re-execution - but does not clear the bound parameters.
	 * This is a wrapper for sqlite3_reset.
	 *
	 * Does not throw.
	 */
	void reset();

	/**
	 * Clears the parameter bindings from the statement, setting all
	 * to NULL. This is a wrapper for sqlite3_clear_bindings.
	 *
	 * Does not throw.
	 */
	void clear_bindings();


private:

	boost::shared_ptr<detail::SQLStatement> m_sql_statement;
};


// Member function templates and inline functions

inline
void
SharedSQLStatement::bind(std::string const& parameter_name, int value)
{
	m_sql_statement->bind(parameter_name, value);
	return;
}


inline
void
SharedSQLStatement::bind
(	std::string const& parameter_name, boost::int64_t value
)
{
	m_sql_statement->bind(parameter_name, value);
	return;
}


inline
void
SharedSQLStatement::bind
(	std::string const& parameter_name, std::string const& value
)
{
	m_sql_statement->bind(parameter_name, value);
	return;
}




template <typename T>
inline
T
SharedSQLStatement::extract(int index)
{
	return m_sql_statement->template extract<T>(index);
}


inline
bool
SharedSQLStatement::step()
{
	return m_sql_statement->step();
}


inline
void
SharedSQLStatement::reset()
{
	m_sql_statement->reset();
	return;
}


inline
void
SharedSQLStatement::clear_bindings()
{
	m_sql_statement->clear_bindings();
	return;
}


}  // namespace sqloxx





#endif  // shared_sql_statement_hpp
