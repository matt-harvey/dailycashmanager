// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_graphical_session_hpp_08719753897152727
#define GUARD_graphical_session_hpp_08719753897152727

#include "session.hpp"
#include <string>


namespace phatbooks
{

/**
 * Namespace specific to the graphical user interface for Phatbooks
 */
namespace gui
{

// We eschew wxWidgets' macro-created main function, and
// instead drive everything from our own main defined in main.cpp
// at the top-level project directory, via our GraphicalSession class
// (a subclass of phatbooks::Session). GraphicalSession is the top-level
// "driver" of the GUI application.
// However we still need to go through the motions of creating an "App"
// class in which to contain all our wxWidgets-using code. The "App"
// class is a separate class inheriting from wxApp. The App class
// is invoked from within GraphicalSession. This is a somewhat
// convoluted initialization architecture, but was originally designed
// to facilitate having both GUI and non-GUI interface running over
// the same core business logic.

class GraphicalSession: public Session
{
public:

	GraphicalSession();
	GraphicalSession(GraphicalSession const&) = delete;
	GraphicalSession(GraphicalSession&&) = delete;
	GraphicalSession& operator=(GraphicalSession const&) = delete;
	GraphicalSession& operator=(GraphicalSession&&) = delete;
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
	int do_run() override;

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
	int do_run(std::string const& filepath_str) override;

	bool m_existing_application_instance_notified;
};

}  // namesapce gui
}  // namespace phatbooks


#endif  // GUARD_graphical_session_hpp_08719753897152727
