// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_side_hpp_6396077878957651
#define GUARD_transaction_side_hpp_6396077878957651

namespace phatbooks
{

/**
 * Represents the notion of a particular side of a
 * Journal, being either the "source" or the "destination"
 * side. "Source" transactions are usually CR and "destination"
 * usually DR; but this need not always be the case. In particular,
 * the signs may be the other way around in case we are reversing a
 * previous transaction.
 */
enum class TransactionSide: unsigned char
{
	source = 0,
	destination,
	num_transaction_sides  // do not insert enumerators below here
};

}  // namespace phatbooks


#endif  // GUARD_transaction_side_hpp_6396077878957651
