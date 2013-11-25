/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_dcm_exceptions_hpp_9435394075436235
#define GUARD_dcm_exceptions_hpp_9435394075436235

#include <jewel/exception.hpp>

/** @file dcm_exceptions.hpp
 *
 * @brief Header file containing exception classes defined in the dcm
 * namespace. These exceptions are for throwing by DCM-specific
 * functions (although DCM-specific functions need not throw \e only
 * these exceptions).
 *
 * These exceptions are defined using the JEWEL_DERIVED_EXCEPTION macro from
 * the Jewel library. They all have \c jewel::Exception as a (direct or
 * indirect) base class.
 */

namespace dcm
{

/**
 * Base class for all the exception classes defined in the \c dcm
 * namespace.
 */
JEWEL_DERIVED_EXCEPTION(DcmException, jewel::Exception);

/**
 * Exception to be thrown when program attempts to execute code which
 * depends on some functionality which has not yet been implemented.
 * This exception should not feature in release builds.
 */
JEWEL_DERIVED_EXCEPTION
(	UnimplementedFunctionalityException,
	jewel::Exception
);

/**
 * Exception to be thrown when an object has not met certain preconditions
 * for being stored in the database.
 */
JEWEL_DERIVED_EXCEPTION(StoragePreconditionsException, DcmException);

/**
 * Exception to be thrown when an invalid conversion is attempted between
 * one date format and another.
 */
JEWEL_DERIVED_EXCEPTION(DateConversionException, DcmException);

/**
 * Exception to be thrown when an unsafe arithmetic operation (addition,
 * subtraction etc.) would be performed.
 */
JEWEL_DERIVED_EXCEPTION(UnsafeArithmeticException, DcmException);

/**
 * Exception to be thrown when a numerical amount (especially, but not
 * necessarily, a jewel::Decimal) has a precision which is an invalid
 * precision in some given context.
 */
JEWEL_DERIVED_EXCEPTION(PrecisionException, DcmException);

/**
 * Exception to be thrown when a Journal is not valid in some context.
 */
JEWEL_DERIVED_EXCEPTION(InvalidJournalException, DcmException);

/**
 * Exception to be thrown when an action is attempted on a Journal (or
 * OrdinaryJournal or DraftJournal) which requires that Journal to be
 * in a balanced state, and that Journal is not in a balanced state.
 */
JEWEL_DERIVED_EXCEPTION(UnbalancedJournalException, InvalidJournalException);

/**
 * Exception to be thrown when a PersistentJournal is such that posting it
 * would cause overflow or unacceptable precision loss in Account balances.
 */
JEWEL_DERIVED_EXCEPTION(JournalOverflowException, InvalidJournalException);

/**
 * Exception to be thrown when a Frequency is not supported or valid
 * in some context.
 */
JEWEL_DERIVED_EXCEPTION(InvalidFrequencyException, DcmException);

/**
 * Exception to be thrown when an attempt is made to get the
 * balancing account from AmalgamatedBudget when it has not
 * been initialized.
 */
JEWEL_DERIVED_EXCEPTION
(	UninitializedBalancingAccountException,
	DcmException
);

/**
 * Exception to be thrown when an attempt is made to edit budgets
 * in an invalid way.
 */
JEWEL_DERIVED_EXCEPTION(BudgetEditingException, DcmException);

/**
 * Exception to be thrown when a BudgetItem is invalid in some context.
 */
JEWEL_DERIVED_EXCEPTION(InvalidBudgetItemException, DcmException);

/**
 * Exception to be thrown when there is an attempt made to delete the
 * persistent database record of a sqloxx::PersistentObject (or derived
 * class e.g. dcm::Journal)
 * that should not be deleted from the database.
 */
JEWEL_DERIVED_EXCEPTION(PreservedRecordDeletionException, DcmException);

/**
 * Exception to be thrown if an attempt is made to set the date of an
 * OrdinaryJournal to a prohibited date.
 */
JEWEL_DERIVED_EXCEPTION(InvalidJournalDateException, DcmException);

/**
 * Exception to be thrown if an attempt is made to set the next_date
 * of a Repeater to a prohibited date.
 */
JEWEL_DERIVED_EXCEPTION(InvalidRepeaterDateException, DcmException);

/**
 * Exception to be thrown when there is an attempt to do something
 * "invalid" with the entity creation date of an accounting entity.
 */
JEWEL_DERIVED_EXCEPTION(EntityCreationDateException, DcmException);

/**
 * Exception to be thrown when something purporting to be the name of
 * an Account is not in fact the name of an Account.
 */
JEWEL_DERIVED_EXCEPTION(InvalidAccountNameException, DcmException);

/**
 * Exception to be thrown when an Account or a range of Accounts
 * is invalid in some context.
 */
JEWEL_DERIVED_EXCEPTION(InvalidAccountException, DcmException);

/**
 * Exception to be thrown when there is an attempt to name an Account with
 * a name that has already been used.
 */
JEWEL_DERIVED_EXCEPTION(DuplicateAccountNameException, DcmException);

/**
 * Exception to be thrown when something is about a Commodity puts it
 * in a state whereby it cannot validly be the default Commodity for the
 * entity represented by a DcmDatabaseConnection.
 */
JEWEL_DERIVED_EXCEPTION(InvalidDefaultCommodityException, DcmException);

/**
 * Exception be thrown by CurrencyManager when exceptional behaviour occurs
 * that is specifically related to that class.
 */
JEWEL_DERIVED_EXCEPTION(CurrencyManagerException, DcmException);

/**
 * Exception to be thrown by DefaultAccountGenerator when exceptional
 * behaviour occurs that is specifically related to that class.
 */
JEWEL_DERIVED_EXCEPTION(DefaultAccountGeneratorException, DcmException);

/**
 * Exception to be thrown when a string does not correspond to any
 * AccountType, but purports to do so.
 */
JEWEL_DERIVED_EXCEPTION
(	InvalidAccountTypeStringException,
	DcmException
);

/**
 * Exception to be thrown when an AccountType or a range of AccountTypes
 * is invalid in a given context.
 */
JEWEL_DERIVED_EXCEPTION(InvalidAccountTypeException, DcmException);

/**
 * Exception to be thrown by AccountDataViewModel when exceptional behaviour
 * occurs that is specifically related to that class.
 */
JEWEL_DERIVED_EXCEPTION(AccountDataViewModelException, DcmException);

/**
 * Exception to be thrown when something that is supposed to represent a
 * date, does not in fact represent a valid date.
 */
JEWEL_DERIVED_EXCEPTION(InvalidDateException, DcmException);

/**
 * Exception to be thrown when something that is supposed to represent a
 * TransactionType, does not in fact represent a TransactionType.
 */
JEWEL_DERIVED_EXCEPTION(InvalidTransactionTypeException, DcmException);

/**
 * Exception to be thrown when a unique name cannot be found for some operation
 * which requires a unique name to be generated in order to succeed.
 */
JEWEL_DERIVED_EXCEPTION(UniqueNameException, DcmException);

}  // namespace dcm

#endif  // GUARD_dcm_exceptions_hpp_9435394075436235

