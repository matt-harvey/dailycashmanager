#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "session.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <string>

using sqloxx::InvalidFilename;
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


int
Session::run(std::string const& p_filename)
{
	if (p_filename.empty())
	{
		throw InvalidFilename("Filename is empty string.");
	}
	return do_run(p_filename);
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
