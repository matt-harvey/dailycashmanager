#ifndef shared_sql_statement_hpp
#define shared_sql_statement_hpp

#include "sqloxx/sql_statement.hpp"
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


namespace sqloxx
{

class DatabaseConnection;
class SQLStatementManager;


/**
 * Class to handle SQLStatement instances via a cache. The
 * caching is done by SQLStatementManager. When a
 * SharedSQLStatement is destructed, it is automatically
 * reset. However the underlying SQLStatement remains in
 * the cache ready for deployment (unless the cache is full).
 * The details of caching are managed by the SQLStatementManager.
 * The client of SharedSQLStatement just calls the constructor
 * and uses the statement.
 *
 * @todo HIGH PRIORITY Provide the means for calling the usual
 * methods in SQLStatement, i.e. bind, step etc.. These should
 * simply be delegated to the underlying SQLStatement. The
 * SharedSQLStatement is simply a handler.
 */
class SharedSQLStatement
{
public:
	
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


	template <typename T>
	T extract(int index);

	bool step();

	void quick_step();

private:

	boost::shared_ptr<SQLStatement> m_sql_statement;
};


// Member function templates and inline functions

inline
SharedSQLStatement::~SharedSQLStatement()
{
	m_sql_statement->reset();
}


inline
void
SharedSQLStatement::bind(std::string const& parameter_name, int value)
{
	m_sql_statement->bind(parameter_name, value);
	return;
}


inline
void
SharedSQLStatement::bind(std::string const& parameter_name, boost::int64_t value)
{
	m_sql_statement->bind(parameter_name, value);
	return;
}


inline
void
SharedSQLStatement::bind(std::string const& parameter_name, std::string const& value)
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
SharedSQLStatement::quick_step()
{
	m_sql_statement->quick_step();
	return;
}


}  // namespace sqloxx





#endif  // shared_sql_statement_hpp
