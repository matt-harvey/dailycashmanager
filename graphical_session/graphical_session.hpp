// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_graphical_session_hpp
#define GUARD_graphical_session_hpp

#include "session.hpp"
#include <string>


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

	/**
	 * Notify session of existing application instance (which could
	 * be either a console or a graphical session), so that just after
	 * the wxWidgets initialization code has run, it can display an
	 * appropriate message box and abort.
	 */
	void notify_existing_application_instance();

private:

	/**
	 * Similar to do_run(std::string const&), but a filepath is not
	 * provided. The user may open a file from within the Session.
	 */
	int do_run();

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
	int do_run(std::string const& filepath_str);

	bool m_existing_application_instance_notified;
};



}  // namesapce gui
}  // namespace phatbooks


#endif  // GUARD_graphical_session_hpp
