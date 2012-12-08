#ifndef sqloxx_exceptions_hpp
#define sqloxx_exceptions_hpp

/** \file sqloxx_exceptions.hpp
 *
 * \brief Header file containing declarations and definitions
 * of exception classes specific to Sqloxx (a wrapper around SQLite).
 *
 * See the Jewel library, in <jewel/exception.hpp> for
 * API of jewel::Exception. All exceptions in sqloxx namespace are
 * ultimately derived from jewel::Exception. The declaration and definition
 * of each exception class derived here is facilitated by the macro
 * JEWEL_DERIVED_EXCEPTION, also defined in <jewel/exception.hpp>.
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
 * Exception to be thrown to avert arithmetic overflow.
 */
JEWEL_DERIVED_EXCEPTION(OverflowException, jewel::Exception);

/**
 * Exception to be thrown when searching for a key in a map
 * or other container, and the key is not found, when it is
 * expected to be.
 */
JEWEL_DERIVED_EXCEPTION(KeyNotFoundException, jewel::Exception);

/**
 * Exception to be thrown when attempting to dereference an
 * unbound Handle<T>
 */
JEWEL_DERIVED_EXCEPTION(UnboundHandleException, jewel::Exception);

/**
 * Exception to be thrown in lieu of std::logic_error (which
 * may not be exception safe).
 */
JEWEL_DERIVED_EXCEPTION(LogicError, jewel::Exception);

/**
 * Exception to be thrown in response to exceptions
 * originating in database-related code. See
 * Jewel library, JEWEL_DERIVED_EXCEPTION for API, as well as the
 * base class, jewel::Exception, also deined in the Jewel library.
 */
JEWEL_DERIVED_EXCEPTION(DatabaseException, jewel::Exception);

/**
 * Exception to be thrown when expected database constraints are believed
 * to have been violated, indicating a possibly corrupted database.
 */
JEWEL_DERIVED_EXCEPTION(ConstraintException, DatabaseException);

/**
 * Exception to be thrown when, for a given type T, and a given
 * database connection, there is an attempt
 * to create multiple instances of IdentityMap<T, Connection> for
 * that database connection.
 */
JEWEL_DERIVED_EXCEPTION(MultipleMapsException, DatabaseException);

/**
 * Exception to be thrown when a database transaction cannot be
 * cleanly finalized.
 */
JEWEL_DERIVED_EXCEPTION(UnresolvedTransactionException, DatabaseException);

/**
 * Exception to be thrown when trying to do something with an invalid database
 * connection.
 */
JEWEL_DERIVED_EXCEPTION(InvalidConnection, DatabaseException);

/**
 * Exception to be thrown when filename passed to \c open method is invalid
 */
JEWEL_DERIVED_EXCEPTION(InvalidFilename, DatabaseException);

/**
 * Exception to be thrown when trying to open connection to one file when
 * already connected to another file
 */
JEWEL_DERIVED_EXCEPTION(MultipleConnectionException, DatabaseException);

/**
 * Exception to be thrown when no result row is expected to remain
 * after stepping through a SQL statement, but nevertheless (at least)
 * one result row remains.
 */
JEWEL_DERIVED_EXCEPTION(UnexpectedResultRow, DatabaseException);

/**
 * Exception to be thrown when a function expects a database table to have
 * a primary key, but it is found not to have one.
 */
JEWEL_DERIVED_EXCEPTION(NoPrimaryKeyException, DatabaseException);

/**
 * Exception to be thrown when a function expects a database table to have
 * an autoincrementing primary key, but it is found not to have one.
 */
JEWEL_DERIVED_EXCEPTION(NoAutoKeyException, DatabaseException);

/**
 * Exception to be thrown when a function expects a database table to have
 * a single-column primary key, but instead finds it has a compound primary
 * key.
 */
JEWEL_DERIVED_EXCEPTION(CompoundPrimaryKeyException, DatabaseException);

/**
 * Exception to be thrown when a table is found to have reached some kind
 * of size limit, but a function expects there still to be room.
 */
JEWEL_DERIVED_EXCEPTION(TableSizeException, DatabaseException);

/**
 * Exception to throw when a result row is not available to a function that
 * expects a result row
 */
JEWEL_DERIVED_EXCEPTION(NoResultRowException, DatabaseException);

/**
 * Exception to throw when an index into a result row is out of range.
 */
JEWEL_DERIVED_EXCEPTION(ResultIndexOutOfRange, DatabaseException);

/**
 * Exception to be thrown when an incorrect assumption is made about the
 * type (SQLite integer, text etc.) of a particular value stored in a
 * database.
 */
JEWEL_DERIVED_EXCEPTION(ValueTypeException, DatabaseException);

/**
 * Exception to be thrown when there is an error in transaction nesting.
 */
JEWEL_DERIVED_EXCEPTION(TransactionNestingException, DatabaseException);

/**
 * Exception to be thrown when SQL statements are passed to function that
 * expects only one.
 */
JEWEL_DERIVED_EXCEPTION(TooManyStatements, DatabaseException);

/**
 * Exception to be thrown in response to database-related exceptions
 * originating in SQLite.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteException, DatabaseException);

/**
 * Exception to be thrown when SQLite (the library itself, not a database
 * connection) has not been successfully initialized.
 */
JEWEL_DERIVED_EXCEPTION(SQLiteInitializationError, SQLiteException);


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
