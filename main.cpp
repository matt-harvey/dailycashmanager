
/** \file main.cpp
 *
 * \brief Contains main.cpp function, and possibly some small amount
 * of other code, relating to Phatbooks application.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



#include "phatbooks_text_session.hpp"
#include <iostream>
#include <string>

using phatbooks::PhatbooksTextSession;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " FILENAME" << endl;
		return 1;
	}
	string filename(argv[1]);
	if (filename.empty())
	{
		cout << "FILENAME cannot be empty string." << endl;
		return 1;
	}

	// The following assumes a text based session.
	assert (argc == 2 && !filename.empty());
	PhatbooksTextSession session;
	int const ret = session.run(filename);
	return ret;
}
