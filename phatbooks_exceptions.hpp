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

/**
 * Exception to be thrown when a Frequency is not supported or valid
 * in some context.
 */
JEWEL_DERIVED_EXCEPTION(InvalidFrequencyException, PhatbooksException);

/**
 * Exception to be thrown when an attempt is made to get the
 * balancing account from AmalgamatedBudget when it has not
 * been initialized.
 */
JEWEL_DERIVED_EXCEPTION
(	UninitializedBalancingAccountException,
	PhatbooksException
);

/**
 * Exception to be thrown when there is an attempt made to delete the
 * persistent database record of a PhatbooksPersistentObject (or
 * derived class) instance, that should not be deleted from the
 * database.
 */
JEWEL_DERIVED_EXCEPTION(PreservedRecordDeletionException, PhatbooksException);

/**
 * Exception to be thrown if an attempt is made to set the date of an
 * OrdinaryJournal to a prohibited date.
 */
JEWEL_DERIVED_EXCEPTION(InvalidJournalDateException, PhatbooksException);

/**
 * Exception to be thrown if an attempt is made to set the next_date
 * of a Repeater to a prohibited date.
 */
JEWEL_DERIVED_EXCEPTION(InvalidRepeaterDateException, PhatbooksException);

/**
 * Exception to be thrown when there is an attempt to do something
 * "invalid" with the entity creation date of an accounting entity.
 */
JEWEL_DERIVED_EXCEPTION(EntityCreationDateException, PhatbooksException);

/**
 * Exception to be thrown when something purporting to be the name of
 * an Account is not in fact the name of an Account.
 */
JEWEL_DERIVED_EXCEPTION(InvalidAccountNameException, PhatbooksException);

/**
 * Exception to be thrown when something is about a Commodity puts it
 * in a state whereby it cannot validly be the default Commodity for the
 * entity represented by a PhatbooksDatabaseConnection.
 */
JEWEL_DERIVED_EXCEPTION(InvalidDefaultCommodityException, PhatbooksException);

/**
 * Exception be thrown by CurrencyManager when exceptional behaviour occurs
 * that is specifically related to that class.
 */
JEWEL_DERIVED_EXCEPTION(CurrencyManagerException, PhatbooksException);

/**
 * Exception to be thrown by DefaultAccountGenerator when exceptional
 * behaviour occurs that is specifically related to that class.
 */
JEWEL_DERIVED_EXCEPTION(DefaultAccountGeneratorException, PhatbooksException);

}  // namespace phatbooks

#endif  // GUARD_phatbooks_exceptions_hpp

