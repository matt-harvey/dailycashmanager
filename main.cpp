
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

#include "phatbooks_text_session.hpp"
#include <cassert>
#include <iostream>
#include <string>

using phatbooks::tui::PhatbooksTextSession;
using std::cout;
using std::endl;
using std::string;


int main(int argc, char** argv)
{
	try
	{
		if (argc != 2)
		{
			cout << "Usage: " << argv[0] << " FILENAME" << endl;
			return 1;
		}
		string const filename(argv[1]);
		if (filename.empty())
		{
			cout << "FILENAME cannot be empty." << endl;
			return 1;
		}

		assert (argc == 2 && !filename.empty());

		// The following assumes a text based session.
		PhatbooksTextSession session;
		return session.run(filename);
	}
	// This seems pointless but is necessary to guarantee the stack is fully
	// unwound if an exception is thrown.
	catch (...)
	{
		throw;
	}
}
