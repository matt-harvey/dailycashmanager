
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

// TODO In due course, we need to reinstate the -mwindows flag
// (see note in CMakeLists.txt).

// TODO Create a decent icon for the application. We want this
// in both .ico form (for Windows executable icon) and .xpm
// form (for icon for MyFrame). Note, when I exported my
// "token icon" using GIMP to .ico format, and later used this
// to create a double-clickable icon in Windows, only the text
// on the icon appeared, and the background image became
// transparent for some reason.

// TODO On Fedora, recompile and install wxWidgets with an additional
// configure flag, viz. --with-gnomeprint (sp?).

// TODO Find way to lock database so that multiple instances of
// Phatbooks can't access it simultaneously.

#include "application.hpp"
#include "b_string.hpp"
#include "graphical_session.hpp"
#include "phatbooks_text_session.hpp"
#include <tclap/CmdLine.h>
#include <cassert>
#include <iostream>
#include <string>

using phatbooks::Application;
using phatbooks::BString;
using phatbooks::bstring_to_std8;
using phatbooks::gui::GraphicalSession;
using phatbooks::tui::PhatbooksTextSession;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using TCLAP::ArgException;
using TCLAP::CmdLine;
using TCLAP::SwitchArg;
using TCLAP::UnlabeledValueArg;


int main(int argc, char** argv)
{
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
		PhatbooksTextSession text_session;
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
