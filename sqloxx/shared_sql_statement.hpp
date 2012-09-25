#ifndef shared_sql_statement_hpp
#define shared_sql_statement_hpp

#include <boost/shared_ptr.hpp>
#include <string>
#include "sqloxx/sql_statement.hpp"


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

	template <typename T>
	void bind(std::string const& parameter_name, T value);

	template <typename T>
	T extract(int index);

	bool step();

	bool quick_step();

private:

	boost::shared_ptr<SQLStatement> m_sql_statement;
};


}  // namespace sqloxx


// Member function templates and inline functions

inline
SharedSQLStatement::~SharedSQLStatement()
{
	m_sql_statement->reset();
}


template <typename T>
inline
void
SharedSQLStatement::bind(std::string const& parameter_name, T value)
{
	m_sql_statement->bind(parameter_name, value);
	return;
}


template <typename T>
inline
T
SharedSQLStatement::extract(int index)
{
	return m_sql_statement.extract(index);
}


inline
bool
SharedSQLStatement::step()
{
	return m_sql_statement->step();
}


inline
bool
SharedSQLStatement::quick_step()
{
	return m_sql_statement->quick_step();
}






#endif  // shared_sql_statement_hpp
