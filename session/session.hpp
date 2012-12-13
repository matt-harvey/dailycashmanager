#ifndef GUARD_session_hpp
#define GUARD_session_hpp

#include <boost/shared_ptr.hpp>
#include <string>

namespace phatbooks
{

// Forward declaration
class PhatbooksDatabaseConnection;

class Session
{
public:
	Session();
	virtual ~Session();
	int run(std::string const& p_filename);
protected:
	static std::string application_name();
	PhatbooksDatabaseConnection& database_connection();
private:
	virtual int do_run(std::string const& p_filename) = 0;
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
};

}  // namespace phatbooks


#endif  // GUARD_session_hpp
