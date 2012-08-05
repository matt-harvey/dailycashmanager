
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

using phatbooks::PhatbooksTextSession;

int main()
{
	// The following assumes a text based session.
	
	PhatbooksTextSession session;
	session.run();


	return 0;
}
