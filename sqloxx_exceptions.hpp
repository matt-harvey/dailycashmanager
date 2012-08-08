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


#include <jewel/exception.hpp>

namespace sqloxx
{


/**
 * Exception to be thrown in response to exceptions
 * originating in database-related code. See
 * Jewel library, JEWEL_STANDARD_EXCEPTION for API.
 */
JEWEL_DERIVED_EXCEPTION(DatabaseException, jewel::Exception);

/**
 * Exception to be thrown in response to database-related exceptions
 * originating in SQLite. See Jewel library, JEWEL_DERIVED_EXCEPTION for API.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteException, DatabaseException);

/**
 * Exception to be thrown when trying to do something with an invalid database
 * connection.
 */
JEWEL_DERIVED_EXCEPTION(InvalidConnection, DatabaseException);

/**
 * Exception to be thrown when SQLite (the library itself, not a database
 * connection) has not been successfully initialized.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteInitializationError, SQLiteException);

/**
 * Exception to be thrown when attempt is made to execute a \c SQLStatement
 * in one step, via \c DatabaseConnection::SQLStatement::quick_step, but there
 * is at least one result row, meaning that \c quick_step is an inappropriate
 * means of execution.
 */
JEWEL_DERIVED_EXCEPTION(UnexpectedResultSet, DatabaseException);

/* NOT DOXYGEN AS DOXYGEN DOESN'T HANDLE
 * The following exceptions correspond to particular SQLite standard error
 * codes. See the SQLite standard documentation for description of what the
 * corresponding error codes signify. The pattern for naming these exceptions
 * is as per the following example: the SQLite error code \c SQLITE_READONLY
 * corresponds to the exception class \c SQLiteReadOnly.
 */

JEWEL_DERIVED_EXCEPTION(SQLiteError, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteInternal, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLitePerm, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteAbort, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteBusy, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteLocked, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteNoMem, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteReadOnly, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteInterrupt, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteIOErr, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteCorrupt, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteFull, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteCantOpen, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteEmpty, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteSchema, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteTooBig, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteConstraint, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteMismatch, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteMisuse, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteNoLFS, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteAuth, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteFormat, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteRange, SQLiteException);
JEWEL_DERIVED_EXCEPTION(SQLiteNotADB, SQLiteException);

/**
 * Exception to be thrown
 * when the SQLite error code is \e not SQLITE_OK, but is also none of
 * the other known error codes.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteUnknownErrorCode, SQLiteException);


}  // namespace sqloxx

#endif  // sqloxx_exceptions_hpp
