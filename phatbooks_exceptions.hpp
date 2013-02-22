#ifndef GUARD_phatbooks_exceptions_hpp
#define GUARD_phatbooks_exceptions_hpp

#include <jewel/exception.hpp>

/** \file phatbooks_exceptions.hpp
 *
 * \brief Header file containing exception classes defined in the phatbooks
 * namespace. These exceptions are for throwing by Phatbooks-specific
 * functions (although Phatbooks-specific functions need not throw \e only
 * these exceptions).
 *
 * These exceptions are defined using the JEWEL_DERIVED_EXCEPTION macro from
 * the Jewel library. They all have \c jewel::Exception as a (direct or
 * indirect) base class.
 *
 * @todo Find out how to catch all exceptions in main and then produce a core
 * dump before aborting. This must be something that the user can then send
 * to a developer to enable error diagnosis.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

namespace phatbooks
{

/**
 * Base class for all the exception classes defined in the \c phatbooks
 * namespace.
 */
JEWEL_DERIVED_EXCEPTION(PhatbooksException, jewel::Exception);

/**
 * Exception to be thrown when an object has not met certain preconditions
 * for being stored in the database.
 */
JEWEL_DERIVED_EXCEPTION(StoragePreconditionsException, PhatbooksException);

/**
 * Exception to be thrown when an invalid conversion is attempted between
 * one date format and another.
 */
JEWEL_DERIVED_EXCEPTION(DateConversionException, PhatbooksException);

/**
 * Exception to be thrown when an unsafe arithmetic operation (addition,
 * subtraction etc.) would be performed.
 */
JEWEL_DERIVED_EXCEPTION(UnsafeArithmeticException, PhatbooksException);

/**
 * Exception to be thrown when an action is attempted on a Journal (or
 * OrdinaryJournal or DraftJournal) which requires that Journal to be
 * in a balanced state, and that Journal is not in a balanced state.
 */
JEWEL_DERIVED_EXCEPTION(UnbalancedJournalException, PhatbooksException);

/**
 * Exception to be thrown when an attempt is made to create an additional
 * instance of phatbooks::Session, after the maximum number of such
 * instances has already been reached.
 */
JEWEL_DERIVED_EXCEPTION(TooManySessions, PhatbooksException);

}  // namespace phatbooks

#endif  // GUARD_phatbooks_exceptions_hpp

