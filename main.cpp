
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

// TODO Make the installer create an association on the user's system
// between the Phatbooks file extension and the Phatbooks application.
// See CMake book, page. 162.

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
// form (for icon for MyFrame). Note, when I exported my
// "token icon" using GIMP to .ico format, and later used this
// to create a double-clickable icon in Windows, only the text
// on the icon appeared, and the background image became
// transparent for some reason. Furthermore, when set as the
// large in-windows icon in the CPack/NSIS installer, the icon
// wasn't showing at all.

// TODO On Fedora, recompile and install wxWidgets with an additional
// configure flag, viz. --with-gnomeprint (sp?).

// TODO Find way to lock database so that multiple instances of
// Phatbooks can't access it simultaneously. In Windows, I think
// this can be done via the CPack/NSIS installer configuration - see
// NSIS documentation.

// TODO Within the Windows Control Panel's list of programs, and possibly
// elsewhere, the version of Phatbooks shows as v0.1.1. This looks
// unprofessional. Set the version number explicitly in CMakeLists.txt,
// (and in Tags in svn) and ensure this carries through consistently to
// all places where it might be seen by users or potential users (e.g.
// the website, the licence, the installer...).

#include "application.hpp"
#include "b_string.hpp"
#include "graphical_session.hpp"
#include "text_session.hpp"
#include <tclap/CmdLine.h>
#include <cassert>
#include <ios>
#include <iostream>
#include <string>

using phatbooks::Application;
using phatbooks::BString;
using phatbooks::bstring_to_std8;
using phatbooks::gui::GraphicalSession;
using phatbooks::tui::TextSession;
using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::string;
using TCLAP::ArgException;
using TCLAP::CmdLine;
using TCLAP::SwitchArg;
using TCLAP::UnlabeledValueArg;

// TODO We need a proper solution to the potential for integer overflow.
// Mostly we use jewel::Decimal arithmetic - which will throw if unsafe -
// but we're not actually handling these exceptions for the user. The
// program would just crash. (And eventually will probably want to use
// SQL summing rather than summing jewel::Decimal in BalanceCache::refresh;
// and I'm not sure that throws at all if unsafe.)


int main(int argc, char** argv)
{
	cout.exceptions(std::ios::badbit | std::ios::failbit);
	cerr.exceptions(std::ios::badbit | std::ios::failbit);
	clog.exceptions(std::ios::badbit | std::ios::failbit);

	try
	{
		// Process command line arguments
		BString const application_name = Application::application_name();
		CmdLine cmd(bstring_to_std8(application_name));
		SwitchArg gui_switch
		(	"c",
			"console",
			"Run in console mode (rather than graphical mode)",
			cmd
		);
		UnlabeledValueArg<string> filepath_arg
		(	"FILE",
			"File to open or create",
			false,
			"",
			"string"
		);
		cmd.add(filepath_arg);
		cmd.parse(argc, argv);
		bool const using_console_mode = gui_switch.getValue();
		string const filepath_str = filepath_arg.getValue();
		if (!using_console_mode)
		{
			GraphicalSession graphical_session;
	
			// Note phatbooks::Session currently requires a std::string to
			// be passed here.
			// TODO This may require a wstring or BString if we want to
			// support non-ASCII filenames on Windows. We would need to
			// change the interface with phatbooks::Session.
			if (filepath_str.empty())
			{
				return graphical_session.run();
			}
			assert (!filepath_str.empty());
			return graphical_session.run(filepath_str);
		}
		assert (using_console_mode);
		TextSession text_session;
		if (filepath_str.empty())
		{
			return text_session.run();
		}
		assert (!filepath_str.empty());
		return text_session.run(filepath_str);
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
