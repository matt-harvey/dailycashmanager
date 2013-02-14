
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
// depending on the platform. String could be defined in phatbooks,
// but then the same problem would occur in jewel::Decimal. So the typedef
// might as well be done in jewel. So we would have jewel::String. We would
// also want analogous classes... jewel::Cout, jewel::Cerr, jewel::Clog,
// jewel::Char. We would also want a macro to wrap string literals and chars
// become either "L" or "", depending on the platform. We could use
// JEWEL_TEXT. It's a bit grotesque but conventional, and clearly
// distinguished from _T(), _() and wxT(), which are used by wxWidgets.
// Which raises the question: why not just use wxString and brethren,
// including in jewel, to solve this problem? We have to use them eventually
// in the GUI code anyway. Or what about wxUString, and have 32-bit chars
// everywhere? Well, I would rather stick with standard library types
// in jewel I think. Hmm..
// What about sqloxx? This should NOT be 32-bit chars, as there does not
// appear to be anything in SQLite to support this in the database. It
// would be best to use either wxString etc. there, or jewel::String.
// (Note we will also have to have wide versions of SQLoxx text binding
// functions etc too.)
// For uniformity, we should avoid 32-bit strings elsewhere too, then.
// The question then becomes, do we (a) use wxString and co. everywhere, or
// or (b) use jewel::String everywhere but then wxString in
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
