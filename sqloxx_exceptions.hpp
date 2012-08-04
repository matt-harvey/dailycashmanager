#ifndef sqloxx_exceptions_hpp
#define sqloxx_exceptions_hpp

/** \file sqloxx_exceptions.hpp
 *
 * \brief Header file containing declarations and definitions
 * of exception classes specific to Sqloxx (a wrapper around SQLite).
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include <jewel/exception_macros.hpp>
#include <stdexcept>
#include <string>

namespace sqloxx
{


/**
 * Exception to be thrown in response to exceptions
 * originating in database-related code. See
 * Jewel library, JEWEL_STANDARD_EXCEPTION for API.
 */
JEWEL_STANDARD_EXCEPTION(DatabaseException);

/**
 * Exception to be thrown in response to database-related exceptions
 * originating in SQLite. See Jewel library, JEWEL_DERIVED_EXCEPTION for API.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteException, DatabaseException);




}  // namespace sqloxx

#endif  // sqloxx_exceptions_hpp
