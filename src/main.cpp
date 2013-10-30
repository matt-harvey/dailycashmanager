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

// TODO HIGH PRIORITY Under KDE (at least on Mageia) the way I have set
// up the widths of wxComboBox and wxTextCtrl and wxButton (in various
// controls in which these feature), where they are
// supposed to be the same height, they actually turn out to be slightly
// different heights.

// TODO HIGH PRIORITY Go through all the classes inheriting from
// sqloxx::PersistentObject<...> and ensure the do_save(...) functions in each
// are atomic with respect to the in-memory objects (noting that Sqloxx
// already ensures atomicity with respect to the database), and that they
// conform to the restrictions detailed in the sqloxx::PersistentObject<...>
// API documentation. (Note I have already done this for \e do_load(...)
// functions.)

// TODO MEDIUM PRIORITY Tooltips aren't showing on Windows.

// TODO HIGH PRIORITY Make the installer create an association on the user's
// system between the Phatbooks file extension and the Phatbooks application.
// See CMake book, page. 162.

// TODO HIGH PRIORITY Should the application should automatically create a
// backup file of the database on startup so that the session can be
// recovered if something goes wrong?

// TODO MEDIUM PRIORITY The database file should perhaps have a checksum to
// guard against its contents changing other than via the application.

// TODO HIGH PRIORITY Facilitate automatic checking for updates from user's
// machine, or at least provide an easy process for installing updates
// via NSIS. It appears that the default configuration of CPack/NSIS is
// such that updates will not overwrite existing files. Some manual NSIS
// scripting may be required to enable this. Also take into account that
// the user may have to restart their computer in the event that they have
// files open while the installer (or "updater") is running (although I
// \e think that the default configuration under CPack does this
// automatically).

// TODO HIGH PRIORITY Create the licence. Needs to go both in LICENSE.txt and
// into the NSIS installer (can configure CMakeLists.txt to get the licence text
// from LICENSE.txt). Take care when creating the license, especially re.
// the third party libraries used in the project.

// TODO HIGH PRIORITY Create a decent icon for the application. We want this
// in both .ico form (for Windows executable icon) and .xpm
// form (for icon for Frame). Note, when I exported my
// "token icon" using GIMP to .ico format, and later used this
// to create a double-clickable icon in Windows, only the text
// on the icon appeared, and the background image became
// transparent for some reason. Furthermore, when set as the
// large in-windows icon in the CPack/NSIS installer, the icon
// wasn't showing at all.

// TODO HIGH PRIORITY Create a better name for the application.

// TODO HIGH PRIORITY Set the version number in a single location and find a
// way to ensure this is reflected consistently everywhere it appears
// (website, installer, licence text etc.).

// TODO HIGH PRIORITY We need a proper solution to the potential for integer
// overflow. Mostly we use jewel::Decimal arithmetic - which will throw if
// unsafe - but we're not actually handling these exceptions for the user. The
// program would just crash. In particular, it may be possible for the user
// to save a BudgetItem which both causes the program to crash, and makes it
// so that they can never open their file again, since the act of updating
// the AmalgamatedBudget causes overflow. (I have already prevented this
// from occurring in the case of saving a PersistentJournal that would cause
// overflow.) The user should be prompted to roll over to a new file, before
// this situation can arise, both in the case of BudgetItem and in the case
// of PersistentJournal. (Also, what about Account opening balance changes that
// might cause overflow? But these are always via the posting of a
// PersistentJournal, so...)

// TODO HIGH PRIORITY Make the GUI display acceptably on smaller screen
// i.e. laptop.

// TODO HIGH PRIORITY Make it so that if the program crashes, a dialog is
// displayed which
// preferably allows the user to email the log file and an optional
// user-supplied message to "help desk" with a single mouse click from
// the window itself; or, if not, at least tells the user
// where the log file is and gets recommends they email a particular
// email address with the log file attached. An alternative to email
// is just to use HTTP to send the logfile directly to a server (after
// getting the user's permission). Could use the server to format an
// email. Use third party library, wxCrashReport?

#include "application.hpp"
#include "string_conv.hpp"
#include "gui/graphical_session.hpp"
#include <boost/filesystem.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/on_windows.hpp>
#include <tclap/CmdLine.h>
#include <wx/log.h>
#include <wx/snglinst.h>
#include <wx/string.h>
#include <wx/utils.h>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <typeinfo>

using jewel::Log;
using phatbooks::Application;
using phatbooks::gui::GraphicalSession;
using phatbooks::wx_to_std8;
using std::abort;
using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::ofstream;
using std::set_terminate;
using std::string;
using std::strlen;
using TCLAP::ArgException;
using TCLAP::CmdLine;
using TCLAP::UnlabeledValueArg;

namespace filesystem = boost::filesystem;

namespace
{

	void flush_standard_output_streams()
	{
		JEWEL_LOG_MESSAGE(Log::info, "Flushing standard output streams.");
		cerr.flush();
		clog.flush();
		cout.flush();
		JEWEL_LOG_MESSAGE(Log::info, "Flushed standard output streams.");
		return;
	}

	void my_terminate_handler()
	{
		JEWEL_LOG_MESSAGE(Log::error, "Entered terminate handler.");
		abort();
	}	

	bool ensure_dir_exists(string const& p_directory)
	{
		if (filesystem::exists(p_directory))
		{
			return true;
		}
		return filesystem::create_directory(p_directory);
	}

	void configure_logging()
	{
		Log::set_threshold(Log::trace);
#		ifdef JEWEL_ON_WINDOWS
			string const a("C:\\ProgramData\\");
			string const b("Phatbooks\\");
			string const c("logs\\");
			bool ok = ensure_dir_exists(a);
			if (ok) ok = ensure_dir_exists(a + b);
			if (ok) ok = ensure_dir_exists(a + b + c);
			if (!ok)
			{
				cerr << "Could not create log file." << endl;
				return;
			}
			string const log_dir = a + b + c;
#		else
			string const log_dir = "/tmp/";
#		endif  // JEWEL_ON_WINDOWS
		string const log_name = 
			log_dir + wx_to_std8(Application::application_name()) + ".log";
		Log::set_filepath(log_name);
		return;
	}

}  // end anonymous namespace


int main(int argc, char** argv)
{
	try
	{
		configure_logging();
		JEWEL_LOG_MESSAGE(Log::info, "Configured logging.");
		set_terminate(my_terminate_handler);
		JEWEL_LOG_MESSAGE
		(	Log::info,
			"Terminate handler has been set to my_terminate_handler."
		);

		// Enable exceptions on standard output streams....
		// On second thought, let's not do this. What if some distant
		// library function writes to std::cerr for example in its
		// destructor, for some reason, relying on this action being
		// exception-safe? Let's keep the standard behaviour
		// (i.e. let's comment out the below).
		// cout.exceptions(std::ios::badbit | std::ios::failbit);
		// clog.exceptions(std::ios::badbit | std::ios::failbit);
		// cerr.exceptions(std::ios::badbit | std::ios::failbit);

		// Prevent multiple instances run by the same user
		JEWEL_LOG_TRACE();
		bool another_is_running = false;
		wxString const app_name = Application::application_name();
		wxString const instance_identifier =
			app_name +
			wxString::Format("-%s", wxGetUserId().c_str());
		wxSingleInstanceChecker const checker(instance_identifier);
		if (checker.IsAnotherRunning())
		{
			another_is_running = true;
			// to which we will respond below
		}

		// Process command line arguments
		JEWEL_LOG_TRACE();
		CmdLine cmd
		(	wx_to_std8(Application::application_name()),
			' ',
			wx_to_std8(Application::version())
		);
		UnlabeledValueArg<string> filepath_arg
		(	"FILE",
			"File to open",
			false,
			"",
			"string"
		);
		cmd.add(filepath_arg);
		cmd.parse(argc, argv);

		// TODO MEDIUM PRIORITY This may require a wstring or wxString if we
		// want to support non-ASCII filenames on Windows. We would need to
		// change the interface with phatbooks::Session.
		string const filepath_str = filepath_arg.getValue();
		if (!filepath_str.empty() && !filesystem::exists(filepath_str))
		{
			// NOTE The function GraphicalSession::do_run(std::string const&)
			// relies on the fact that we exit here - we don't want to pass a
			// non-existent filepath to that function.
			cerr << "File does not exist.\n"
			     << "To create a new file using the GUI, run with no command "
				 << "line arguments."
				 << endl;
			return 1;
		}
		GraphicalSession graphical_session;
		if (another_is_running)
		{
			// We tell the GraphicalSession of an existing instance
			// so that it can end this session with a graphical
			// message box, which it can only do after wxWidgets'
			// initialization code has run.
			graphical_session.notify_existing_application_instance();
		}
		// Note phatbooks::Session currently requires a std::string to
		// be passed here.

		if (filepath_str.empty())
		{
			return graphical_session.run();
		}
		JEWEL_ASSERT (!filepath_str.empty());
		return graphical_session.run(filepath_str);
	}
	catch (ArgException& e)
	{
		JEWEL_LOG_MESSAGE(Log::error, "ArgException e caught in main.");
		JEWEL_LOG_VALUE(Log::error, e.error());
		JEWEL_LOG_VALUE(Log::error, e.argId());
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		flush_standard_output_streams();
		return 1;
	}
	catch (jewel::Exception& e)
	{
		JEWEL_LOG_MESSAGE
		(	Log::error,
			"jewel::Exception e caught in main."
		);
		JEWEL_LOG_VALUE(Log::error, e);
		cerr << e << endl;
		if (strlen(e.type()) == 0)
		{
			JEWEL_LOG_VALUE(Log::error, typeid(e).name());
			cerr << "typeid(e).name(): " << typeid(e).name() << '\n' << endl;
		}
		flush_standard_output_streams();
		JEWEL_LOG_MESSAGE(Log::error, "Rethrowing e.");
		throw;
	}
	catch (std::exception& e)
	{
		JEWEL_LOG_MESSAGE(Log::error, "std::exception e caught in main.");
		JEWEL_LOG_VALUE(Log::error, typeid(e).name());
		JEWEL_LOG_VALUE(Log::error, e.what());
		cerr << "EXCEPTION:" << endl;
		cerr << "typeid(e).name(): " << typeid(e).name() << endl;
		cerr << "e.what(): " << e.what() << endl;
		flush_standard_output_streams();
		JEWEL_LOG_MESSAGE(Log::error, "Rethrowing e.");
		throw;
	}

	// This is necessary to guarantee the stack is fully unwound no
	// matter what exception is thrown - we're not ONLY doing it
	// for the logging and flushing.
	catch (...)
	{
		JEWEL_LOG_MESSAGE(Log::error, "Unknown exception caught in main.");
		cerr << "Unknown exception caught in main." << endl;
		flush_standard_output_streams();
		JEWEL_LOG_MESSAGE(Log::error, "Rethrowing unknown exception.");
		throw;
	}
}




