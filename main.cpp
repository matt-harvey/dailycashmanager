
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

// TODO It looks like I can get around the issue of typedefs not working
// within templates by using the typename keyword. This will
// enable proper encapsulation of Id type within each business
// class. The direct use of sqloxx::Id within templates that
// are supposed to be instantiated across different business
// classes, can then be abolished, in favour of a locution of the form
// as T::template Id.

#include "application.hpp"
#include "graphical_session.hpp"
#include "phatbooks_text_session.hpp"
#include <tclap/CmdLine.h>
#include <cassert>
#include <iostream>
#include <string>

using phatbooks::Application;
using phatbooks::gui::GraphicalSession;
using phatbooks::tui::PhatbooksTextSession;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::wstring;
using TCLAP::ArgException;
using TCLAP::CmdLine;
using TCLAP::SwitchArg;
using TCLAP::UnlabeledValueArg;


int main(int argc, char** argv)
{
	try
	{
		// Process command line arguments
		CmdLine cmd("Phatbooks"); // WARNING TEMP - should use Application::application_name()
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
