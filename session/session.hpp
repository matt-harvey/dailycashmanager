#ifndef GUARD_session_hpp
#define GUARD_session_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <list>
#include <string>

namespace phatbooks
{

// Forward declaration
class OrdinaryJournal;
class PhatbooksDatabaseConnection;

/**
 * Phatbooks user session base class.
 * May be inherited by a class representing a more specific kind
 * of session, e.g. a console session or a GUI session.
 */
class Session
{
public:
	Session();
	virtual ~Session();
	int run(std::string const& p_filename);
protected:
	static std::string application_name();
	PhatbooksDatabaseConnection& database_connection();
	boost::shared_ptr<std::list<OrdinaryJournal> > update_repeaters_till
	(	boost::gregorian::date d
	);
private:
	virtual int do_run(std::string const& p_filename) = 0;
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
};

}  // namespace phatbooks


#endif  // GUARD_session_hpp
