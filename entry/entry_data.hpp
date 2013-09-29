#ifndef GUARD_entry_data_hpp_003097491805952737
#define GUARD_entry_data_hpp_003097491805952737

#include "entry.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <boost/optional.hpp>

namespace phatbooks
{


struct Entry::EntryData
{
	boost::optional<sqloxx::Id> journal_id;
	boost::optional<AccountHandle> account;
	boost::optional<wxString> comment;
	boost::optional<jewel::Decimal> amount;
	boost::optional<bool> is_reconciled;
	boost::optional<TransactionSide> transaction_side;
};



}  // namespace phatbooks

#endif  // GUARD_entry_data_hpp_003097491805952737
