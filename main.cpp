
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



#include "phatbooks_text_session.hpp"
#include <cassert>
#include <iostream>
#include <string>

using phatbooks::PhatbooksTextSession;
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
