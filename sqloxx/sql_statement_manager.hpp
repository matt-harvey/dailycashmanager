#ifndef sql_statement_manager_hpp
#define sql_statement_manager_hpp

#include <boost/circular_buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>



class DatabaseConnection;
class SQLStatement;


namespace sloxx
{


class SQLStatementManager
{

public:

	typedef
		boost::circular_buffer< boost::shared_ptr<SQLStatement> >
		History;

	explicit
	SQLStatementManager
	(	boost::shared_ptr<DatabaseConnection> p_database_connection,
		History::size_type p_cache_capacity
	);
	
	void add_statement(boost::shared_ptr<SQLStatement> p_statement);
	
	

private:

	typedef
		std::map< std::string, boost::shared_ptr<SQLStatement> >
		StatementCache;
	
	boost::shared_ptr<DatabaseConnection> m_database_connection;
	StatementCache m_statement_cache;
	History m_statement_history;
};



}  // namespace sqloxx

#endif  // sql_statement_manager_hpp
