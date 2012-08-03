#ifndef sqloxx_exceptions_hpp
#define sqloxx_exceptions_hpp

#include <jewel/exception_macros.hpp>
#include <stdexcept>
#include <string>

namespace sqloxx
{


/**
 * Exception to be thrown in response to exceptions
 * originating in database-related code. See JEWEL_STANDARD_EXCEPTION for API.
 */
JEWEL_STANDARD_EXCEPTION(DatabaseException);

/**
 * Exception to be thrown in response to database-related exceptions
 * originating in SQLite.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteException, DatabaseException);




}  // namespace sqloxx

#endif  // sqloxx_exceptions_hpp
