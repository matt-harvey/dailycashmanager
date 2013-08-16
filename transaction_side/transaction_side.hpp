#ifndef GUARD_transaction_side_hpp
#define GUARD_transaction_side_hpp

namespace phatbooks
{

namespace transaction_side
{
	/**
	 * Represents the notion of a particular side of a
	 * Journal, being either the "source" or the "destination"
	 * side. The source side contains those Entries that are
	 * before the Journal::fulcrum(); and the destination side
	 * contains those Entries that at or later than the Journal::fulcrum()
	 * point.
	 */
	enum TransactionSide
	{
		source,
		destination
	};

};  // namespace transaction_side

}  // namespace phatbooks

#endif  // GUARD_transaction_side_hpp
