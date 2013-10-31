/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_graphical_session_hpp_08719753897152727
#define GUARD_graphical_session_hpp_08719753897152727

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

class GraphicalSession
{
public:
	
	/**
	 * Only 1 GraphicalSession may be created, and this is non-copyable.
	 * So this is the "Singleton pattern" in this sense. However, the single
	 * instance is \e not globally accessible via a static function: if we
	 * require references to it in multiple places, we must explicitly pass
	 * these around.
	 *
	 * @throw TooManySessions if we attempt to create a second GraphicalSession.
	 */
	GraphicalSession();

	GraphicalSession(GraphicalSession const&) = delete;
	GraphicalSession(GraphicalSession&&) = delete;
	GraphicalSession& operator=(GraphicalSession const&) = delete;
	GraphicalSession& operator=(GraphicalSession&&) = delete;
	~GraphicalSession();

	/**
	 * Notify session of existing application instance (which could
	 * be either a console or a graphical session), so that just after
	 * the wxWidgets initialization code has run, it can display an
	 * appropriate message box and abort.
	 */
	void notify_existing_application_instance();

	/**
	 * Run a session in which the user chooses a file to open from
	 * within the session.
	 */
	int run();

	/**
	 * Run a session in which the file to open is given by \e filepath_str.
	 *
	 * @param filename name of file to which a database connection
	 * should be opened for the session.
	 *
	 * @returns \c 1 if there is some kind of error condition that is
	 * not manifested as an exception; or \c 0 on successful completion.
	 */
	int run(std::string const& filepath_str);

private:

	static int const s_max_instances = 1;
	static int const s_default_caching_level = 10;
	static int s_num_instances;

	bool m_existing_application_instance_notified;
};

}  // namesapce gui
}  // namespace phatbooks


#endif  // GUARD_graphical_session_hpp_08719753897152727
