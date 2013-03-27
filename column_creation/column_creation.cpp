#include "column_creation.hpp"

#include "entry.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "ordinary_journal.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <consolixx/alignment.hpp>
#include <consolixx/column.hpp>
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/exception.hpp>
#include <jewel/optional.hpp>
#include <string>

using boost::lexical_cast;
using boost::optional;
using consolixx::AccumulatingColumn;
using consolixx::PlainColumn;
using jewel::Decimal;
using jewel::UninitializedOptionalException;
using jewel::value;
using std::string;

namespace alignment = consolixx::alignment;
namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace column_creation
{


/**
 * Anonymous namespace containing convenient functions to assist
 * in constructing consolixx::Column objects per below.
 */
namespace
{
	// Convenient non-member functions to assist in constructing
	// consolixx::Column objects per below.

	OrdinaryJournal::Id entry_col_aux_ordinary_journal_id(Entry const& entry)
	{
		OrdinaryJournal const journal = entry.journal<OrdinaryJournal>();
		if (!journal.has_id())
		{
			throw UninitializedOptionalException
			(	"Journal does not have id."
			);
		}
		assert (journal.has_id());
		return journal.id();
	}
	gregorian::date entry_col_aux_ordinary_journal_date(Entry const& entry)
	{
		return entry.journal<OrdinaryJournal>().date();
	}
	optional<Entry::Id> entry_col_aux_optional_id(Entry const& entry)
	{
		optional<Entry::Id> ret;
		if (entry.has_id()) ret = entry.id();
		return ret;
	}
	string entry_col_aux_optional_id_to_string(optional<Entry::Id> id)
	{
		if (id)
		{
			return lexical_cast<string>(value(id));
		}
		assert (!id);
		return "N/A";
	}
	BString entry_col_aux_account_name(Entry const& entry)
	{
		return entry.account().name();
	}
	BString entry_col_aux_comment(Entry const& entry)
	{
		return entry.comment();
	}
#	ifdef PHATBOOKS_EXPOSE_COMMODITY
		BString entry_col_aux_commodity_abbreviation(Entry const& entry)
		{
			return entry.account().commodity().abbreviation();
		}
#	endif
	Decimal entry_col_aux_amount(Entry const& entry)
	{
		return entry.amount();
	}
	Decimal entry_col_aux_reversed_amount(Entry const& entry)
	{
		Decimal const amount = entry.amount();
		return round(-amount, amount.places());
	}
	bool entry_col_aux_reconciliation_status(Entry const& entry)
	{
		return entry.is_reconciled();
	}
	Decimal entry_col_aux_accumulate_amount
	(	Entry const& entry,
		Decimal& accumulator
	)
	{
		Decimal const ret = entry.amount();
		accumulator += ret;
		return ret;
	}
	Decimal entry_col_aux_accumulate_reversed_amount
	(	Entry const& entry,
		Decimal& accumulator
	)
	{
		Decimal const raw_amount = entry.amount();
		Decimal const ret = round(-raw_amount, raw_amount.places());
		accumulator += ret;
		return ret;
	}
	Decimal entry_col_aux_running_total_amount
	(	Entry const& entry,
		Decimal& accumulator
	)
	{
		accumulator += entry.amount();
		return accumulator;
	}
	Decimal entry_col_aux_running_total_reconciled_amount
	(	Entry const& entry,
		Decimal& accumulator
	)
	{
		if (entry.is_reconciled())
		{
			accumulator += entry.amount();
		}
		return accumulator;
	}
	string entry_col_aux_reconciliation_status_to_string(bool p_is_reconciled)
	{
		return p_is_reconciled? "y": "n";
	}
	BString account_col_aux_name(Account const& account)
	{
		return account.name();
	}
	account_type::AccountType account_col_aux_account_type
	(	Account const& account
	)
	{
		return account.account_type();
	}
	string account_col_aux_account_type_to_std8
	(	account_type::AccountType type
	)
	{
		return bstring_to_std8(account_type_to_string(type));
	}
	BString account_col_aux_description(Account const& account)
	{
		return account.description();
	}
	Decimal account_col_aux_friendly_balance(Account const& account)
	{
		return account.friendly_balance();
	}
	Decimal account_col_aux_accumulating_friendly_balance
	(	Account const& account,
		Decimal& accumulator
	)
	{
		Decimal const ret = account.friendly_balance();
		accumulator += ret;
		return ret;
	}
	Decimal account_col_aux_budget
	(	Account const& account,
		Decimal& accumulator
	)
	{
		Decimal const ret = account.budget();
		accumulator += ret;
		return ret;
	}


}  // end anonymous namespace
		

PlainColumn<Entry, Journal::Id>*
create_entry_ordinary_journal_id_column()
{
	return new PlainColumn<Entry, Journal::Id>
	(	entry_col_aux_ordinary_journal_id,
		"Journal ID",
		alignment::right
	);
}

PlainColumn<Entry, gregorian::date>*
create_entry_ordinary_journal_date_column()
{
	return new PlainColumn<Entry, gregorian::date>
	(	entry_col_aux_ordinary_journal_date,
		"Date"
	);
}

PlainColumn<Entry, optional<Entry::Id> >*
create_entry_id_column()
{
	return new PlainColumn<Entry, optional<Entry::Id> >
	(	entry_col_aux_optional_id,
		"Entry ID",
		alignment::right,
		entry_col_aux_optional_id_to_string
	);
}

PlainColumn<Entry, BString>*
create_entry_account_name_column()
{
	return new PlainColumn<Entry, BString>
	(	entry_col_aux_account_name,
		"Account",
		alignment::left,
		bstring_to_std8
	);
}

PlainColumn<Entry, BString>*
create_entry_comment_column()
{
	return new PlainColumn<Entry, BString>
	(	entry_col_aux_comment,
		"Comment",
		alignment::left,
		bstring_to_std8
	);
}

#ifdef PHATBOOKS_EXPOSE_COMMODITY
	PlainColumn<Entry, BString>*
	create_entry_commodity_abbreviation_column()
	{
		return new PlainColumn<Entry, BString>
		(	entry_col_aux_commodity_abbreviation,
			"Commodity",
			alignment::left,
			bstring_to_std8
		);
	}
#endif  // PHATBOOKS_EXPOSE_COMMODITY


PlainColumn<Entry, Decimal>*
create_entry_amount_column()
{
	return new PlainColumn<Entry, Decimal>
	(	entry_col_aux_amount,
		"Amount",
		alignment::right,
		finformat_std8
	);
}

PlainColumn<Entry, Decimal>*
create_entry_reversed_amount_column()
{
	return new PlainColumn<Entry, Decimal>
	(	entry_col_aux_reversed_amount,
		"Amount",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Entry, Decimal>*
create_entry_accumulating_amount_column(Decimal const& p_seed)
{
	return new AccumulatingColumn<Entry, Decimal>
	(	entry_col_aux_accumulate_amount,
		p_seed,
		"Amount",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Entry, Decimal>*
create_entry_accumulating_reversed_amount_column(Decimal const& p_seed)
{
	return new AccumulatingColumn<Entry, Decimal>
	(	entry_col_aux_accumulate_reversed_amount,
		p_seed,
		"Amount",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Entry, Decimal>*
create_entry_running_total_amount_column(Decimal const& p_seed)
{
	AccumulatingColumn<Entry, Decimal>* ret =
		new AccumulatingColumn<Entry, Decimal>
		(	entry_col_aux_running_total_amount,
			p_seed,
			"Balance",
			alignment::right,
			finformat_std8
		);
	ret->suppress_footer();
	return ret;
}
	
AccumulatingColumn<Entry, Decimal>*
create_entry_running_total_reconciled_amount_column
(	Decimal const& p_seed
)
{
	AccumulatingColumn<Entry, Decimal>* ret =
		new AccumulatingColumn<Entry, Decimal>
		(	entry_col_aux_running_total_reconciled_amount,
			p_seed,
			"Reconciled balance",
			alignment::right,
			finformat_std8
		);
	ret->suppress_footer();
	return ret;
}

PlainColumn<Entry, bool>*
create_entry_reconciliation_status_column()
{
	return new PlainColumn<Entry, bool>
	(	entry_col_aux_reconciliation_status,
		"Reconciled?",
		alignment::left,
		entry_col_aux_reconciliation_status_to_string
	);
}


PlainColumn<Account, BString>*
create_account_name_column()
{
	return new PlainColumn<Account, BString>
	(	account_col_aux_name,
		"Account",
		alignment::left,
		bstring_to_std8
	);
}

PlainColumn<Account, account_type::AccountType>*
create_account_type_column()
{
	return new PlainColumn<Account, account_type::AccountType>
	(	account_col_aux_account_type,
		"Type",
		alignment::left,
		account_col_aux_account_type_to_std8
	);
}

PlainColumn<Account, BString>*
create_account_description_column()
{
	return new PlainColumn<Account, BString>
	(	account_col_aux_description,
		"Description",
		alignment::left,
		bstring_to_std8
	);
}

PlainColumn<Account, Decimal>*
create_account_friendly_balance_column()
{
	return new PlainColumn<Account, Decimal>
	(	account_col_aux_friendly_balance,
		"Balance",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Account, Decimal>*
create_account_accumulating_friendly_balance_column()
{
	return new AccumulatingColumn<Account, Decimal>
	(	account_col_aux_accumulating_friendly_balance,
		Decimal(0, 0),
		"Balance",
		alignment::right,
		finformat_std8
	);
}

AccumulatingColumn<Account, Decimal>*
create_account_budget_column
(	PhatbooksDatabaseConnection const& p_database_connection
)
{
	Frequency const frequency = p_database_connection.budget_frequency();
	assert (frequency.num_steps() == 1);
	return new AccumulatingColumn<Account, Decimal>
	(	account_col_aux_budget,
		Decimal(0, 0),
		"Budget/" + bstring_to_std8(phrase(frequency.step_type(), false)),
		alignment::right,
		finformat_std8
	);
}


}  // namespace column_creation
}  // namespace phatbooks
