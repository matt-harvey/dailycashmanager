
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


// TODO High priority. I need to make Unicode supportable on both
// Linux and Windows builds.
// I should do this as follows. Have wxString throughout the business
// layer of Phatbooks, both in GUI and TUI code. In Linux, wxString
// is a UTF-8 encoded string, and on Windows, it is a UTF-16 encoded
// wide string.
// At the lowest level just before the database - i.e. in the load() and save()
// methods of the ...Impl classes - I convert from wxString to UTF-8 std::string just
// before storing to the database, and convert from
// UTF-8 std::string to wxString just after retrieving from the
// database. This ensures that strings stored on one platform can be
// read later on another platform and display properly: all strings
// are stored with the same encoding in the database.
//
// This entails a major editing operation throughout the code base.
//
// Note that Jewel currently supports std::string and std::wstring, and
// I don't think I should involve wxString in that. I may need to do
// some conversion to and from wxString and std::string and std::wstring,
// in code that involves jewel::Decimal at the presentation layer.
// 
// Sqloxx currently supports std::string only; and I don't think there's any
// need for it to support anything else. All strings are ultimately going
// to be stored in the database in UTF-8 form so this narrow interface
// can be retained for Sqloxx.

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
