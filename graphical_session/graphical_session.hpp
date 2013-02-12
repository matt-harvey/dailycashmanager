#ifndef GUARD_graphical_session_hpp
#define GUARD_graphical_session_hpp

#include "session.hpp"

namespace phatbooks
{

/**
 * Namespace specific to the graphical user interface for Phatbooks
 */
namespace gui
{

class GraphicalSession:
	public Session  // phatbooks::Session
{
public:
	GraphicalSession();
	virtual ~GraphicalSession();

private:

	/**
	 * Implements virtual function do_run, inherited from
	 * phatbooks::Session.
	 * 
	 * @param filename name of file to which a database connection
	 * should be opened for the session.
	 *
	 * @returns \c 1 if there is some kind of error condition that is
	 * not manifested as an exception; or \c 0 on successful completion.
	 */
	int do_run(std::string const& filename);

};



}  // namesapce gui
}  // namespace phatbooks


#endif  // GUARD_graphical_session_hpp
