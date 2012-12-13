#include "phatbooks_database_connection.hpp"
#include "session.hpp"
#include <string>

using std::string;

namespace phatbooks
{


Session::Session():
	m_database_connection(new PhatbooksDatabaseConnection)
{
	database_connection().set_caching_level(10);
}


Session::~Session()
{
}

string
Session::application_name()
{
	static string const ret = "Phatbooks";
	return ret;
}

PhatbooksDatabaseConnection&
Session::database_connection()
{
	return *m_database_connection;
}


}  // namespace phatbooks
