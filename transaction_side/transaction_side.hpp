#ifndef GUARD_transaction_side_hpp
#define GUARD_transaction_side_hpp

namespace phatbooks
{

namespace transaction_side
{
	/**
	 * Represents the notion of a particular side of a
	 * Journal, being either the "source" or the "destination"
	 * side. "Source" transactions are usually CR and "destination"
	 * usually DR; but this need not always be the case. In particular,
	 * the signs may be the other way around in case we are reversing a
	 * previous transaction.
	 */
	enum TransactionSide
	{
		source = 0,
		destination,
		num_transaction_sides  // do not insert enumerators below here
	};

};  // namespace transaction_side

}  // namespace phatbooks

#endif  // GUARD_transaction_side_hpp
