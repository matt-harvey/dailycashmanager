
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

#include "text_user_session.hpp"

using phatbooks::TextUserSession;

int main()
{
	// The following assumes a text based session.
	
	TextUserSession session;
	session.run();


	return 0;
}
