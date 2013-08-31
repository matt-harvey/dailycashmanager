// Copyright (c) 2013, Matthew Harvey. All rights reserved.


/** \file main.cpp
 *
 * \brief Contains main function, and possibly some small amount
 * of other code, relating to Phatbooks application.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


// TODO HIGH PRIORITY Tooltips aren't showing on Windows.

// TODO Make the installer create an association on the user's system
// between the Phatbooks file extension and the Phatbooks application.
// See CMake book, page. 162.

// TODO (For GUI). Users will probably expect
// right-clicking to cause a context-dependent menu to pop up.

// TODO The application should automatically create a zipped backup
// file so that the session can be recovered if something goes wrong.

// TODO The database file should perhaps have a checksum to guard
// against its contents changing other than via the application.

// TODO Facilitate automatic checking for updates from user's
// machine, as well as easy process for providing updates
// via NSIS. It appears that the default configuration of CPack/NSIS is
// such that updates will not overwrite existing files. Some manual NSIS
// scripting may be required to enable this. Also take into account that
// the user may have to restart their computer in the event that they have
// files open while the installer (or "updater") is running (although I
// \e think that the default configuration under CPack does this
// automatically).

// TODO Write the licence.

// TODO Ensure that option for the user to lauch directly from the
// installer, works correctly.

// TODO Create a decent icon for the application. We want this
// in both .ico form (for Windows executable icon) and .xpm
// form (for icon for Frame). Note, when I exported my
// "token icon" using GIMP to .ico format, and later used this
// to create a double-clickable icon in Windows, only the text
// on the icon appeared, and the background image became
// transparent for some reason. Furthermore, when set as the
// large in-windows icon in the CPack/NSIS installer, the icon
// wasn't showing at all.

// TODO On Fedora, recompile and install wxWidgets with an additional
// configure flag, viz. --with-gnomeprint (sp?).

// TODO Set the version number in a single location and find a way
// to ensure this is reflected consistently everywhere it appears
// (website, installer, licence text etc.).

// TODO We need a proper solution to the potential for integer overflow.
// Mostly we use jewel::Decimal arithmetic - which will throw if unsafe -
// but we're not actually handling these exceptions for the user. The
// program would just crash.

// TODO HIGH PRIORITY Make the GUI display acceptably on smaller screen
// i.e. laptop.

#include "application.hpp"
#include "string_conv.hpp"
#include "graphical_session.hpp"
#include <boost/scoped_ptr.hpp>
#include <tclap/CmdLine.h>
#include <wx/log.h>
#include <wx/snglinst.h>
#include <wx/string.h>
#include <wx/utils.h>
#include <cassert>
#include <ios>
#include <iostream>
#include <string>

using boost::scoped_ptr;
using phatbooks::Application;
using phatbooks::gui::GraphicalSession;
using phatbooks::wx_to_std8;
using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::string;
using TCLAP::ArgException;
using TCLAP::CmdLine;
using TCLAP::UnlabeledValueArg;


int main(int argc, char** argv)
{
	try
	{
		// Enable exceptions on standard streams
		cout.exceptions(std::ios::badbit | std::ios::failbit);
		cerr.exceptions(std::ios::badbit | std::ios::failbit);
		clog.exceptions(std::ios::badbit | std::ios::failbit);

		// Prevent multiple instances run by the same user
		bool another_is_running = false;
		wxString const app_name = Application::application_name();
		wxString const instance_identifier =
			app_name +
			wxString::Format("-%s", wxGetUserId().c_str());
		scoped_ptr<wxSingleInstanceChecker> const m_checker
		(	new wxSingleInstanceChecker(instance_identifier)
		);
		if (m_checker->IsAnotherRunning())
		{
			another_is_running = true;
			// to which we will respond below
		}

		// Process command line arguments
		CmdLine cmd(wx_to_std8(Application::application_name()));
		UnlabeledValueArg<string> filepath_arg
		(	"FILE",
			"File to open or create",
			false,
			"",
			"string"
		);
		cmd.add(filepath_arg);
		cmd.parse(argc, argv);
		string const filepath_str = filepath_arg.getValue();
		GraphicalSession graphical_session;
		if (another_is_running)
		{
			// We tell the GraphicalSession of an existing instance
			// so that it can this session with a graphical
			// message box, which it can only do after wxWidgets'
			// initialization code has run.
			graphical_session.notify_existing_application_instance();
		}
		// Note phatbooks::Session currently requires a std::string to
		// be passed here.
		// TODO This may require a wstring or wxString if we want to
		// support non-ASCII filenames on Windows. We would need to
		// change the interface with phatbooks::Session.
		if (filepath_str.empty())
		{
			return graphical_session.run();
		}
		assert (!filepath_str.empty());
		return graphical_session.run(filepath_str);
	}
	catch (ArgException& e)
	{
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		return 1;
	}
	// This seems pointless but is necessary to guarantee the stack is fully
	// unwound if an exception is thrown.
	catch (...)
	{
		throw;
	}
}




