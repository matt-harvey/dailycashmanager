#ifndef GUARD_transaction_type_hpp
#define GUARD_transaction_type_hpp

namespace phatbooks
{

namespace transaction_type
{
	/**
	 * Different types of "transactions" (i.e., Journals), as they appear
	 * to the \e user. This affects the way we present Journal-related
	 * information to the user, for example whether we use the words
	 * "amount spent", "amount earned" or "amount transferred" when
	 * requesting the transaction amount from the user.
	 */
	enum TransactionType
	{
		expenditure_transaction = 0,
		revenue_transaction,
		balance_sheet_transaction,
		envelope_transaction,
		generic_transaction,
		num_transaction_types  // do not enumerators after here
	};

}  // namespace transaction_type
}  // namespace phatbooks

#endif  // GUARD_transaction_type_hpp
