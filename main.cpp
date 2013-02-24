
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
		SwitchArg gui_switch("g", "gui", "Run in graphical mode", cmd);
		UnlabeledValueArg<string> filepath_arg
		(	"FILE",
			"File to open or create",
			true,
			"",
			"string"
		);
		cmd.add(filepath_arg);
		cmd.parse(argc, argv);
		bool const is_gui = gui_switch.getValue();
		string const filename = filepath_arg.getValue();
		if (is_gui)
		{
			GraphicalSession graphical_session;
	
			// Note phatbooks::Session currently requires a std::string to
			// be passed here.
			// TODO This may require a wstring or BString if we want to
			// support non-ASCII filenames on Windows. We would need to
			// change the interface with phatbooks::Session.
			return graphical_session.run(filename);
		}
		assert (!is_gui);
		PhatbooksTextSession text_session;
		return text_session.run(filename);
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
