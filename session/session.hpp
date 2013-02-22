#ifndef GUARD_session_hpp
#define GUARD_session_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
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
class Session:
	boost::noncopyable
{
public:

	/**
	 * Only 1 Session may be created, and this is non-copyable.
	 * So this is the "Singleton pattern".
	 * However, the single instance is \e not globally accesssible via
	 * a static function: if we require references to it in multiple
	 * places, we must explicitly pass these around.
	 *
	 * @throws TooManySessions if we attempt to create a second
	 * Session.
	 */
	Session();
	virtual ~Session();
	int run(std::string const& p_filename);
	PhatbooksDatabaseConnection& database_connection() const;
protected:
	boost::shared_ptr<std::list<OrdinaryJournal> > update_repeaters_till
	(	boost::gregorian::date d
	);
private:
	virtual int do_run(std::string const& p_filename) = 0;
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
	static int const s_max_instances = 1;
	static int const s_default_caching_level = 10;
	static int s_num_instances;
};

}  // namespace phatbooks


#endif  // GUARD_session_hpp
