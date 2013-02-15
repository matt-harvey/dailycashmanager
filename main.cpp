
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
// Typedef String as either std::string or std::wstring,
// depending on the platform. String could be defined in phatbooks.
// In the Jewel library, I have now provided for both std::string and
// std::wstring, wide streams etc.. So Jewel is all good.
// I should also do the same for sqloxx.
// Then in phatbooks either I could typedef a phatbooks::String class and
// use that, but then convert to wxString where required in the GUI
// layer. Or I could just use wxString throughout phatbooks.
// We also want a macro to wrap string literals and chars
// become either "L" or "", depending on the platform. We could use
// _T(), _() and/or wxT(), which are used by wxWidgets.
// Note we cannot use 32-bit chars as SQLite does not appear to support these.
// For uniformity, we should avoid 32-bit strings elsewhere too, then.
// The question then becomes, do we (a) use wxString and co. everywhere, or
// or (b) use phatbooks::String everywhere but then wxString in
// the GUI code?


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
