// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "budget_item.hpp"
#include "account.hpp"
#include "budget_item_impl.hpp"
#include "string_conv.hpp"
#include "finformat.hpp"
#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "string_conv.hpp"
#include <boost/lexical_cast.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <jewel/output_aux.hpp>
#include <sqloxx/handle.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using boost::lexical_cast;
using jewel::Decimal;
using jewel::round;
using jewel::UninitializedOptionalException;
using sqloxx::Handle;
using std::endl;
using std::ostream;
using std::string;
using std::vector;

namespace phatbooks
{

void
BudgetItem::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	BudgetItemImpl::setup_tables(dbc);
	return;
}

BudgetItem::BudgetItem(PhatbooksDatabaseConnection& p_database_connection):
	PhatbooksPersistentObject(p_database_connection)
{
}
	
BudgetItem::BudgetItem
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}

BudgetItem
BudgetItem::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return BudgetItem
	(	Handle<BudgetItemImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}

void
BudgetItem::set_description(wxString const& p_description)
{
	impl().set_description(p_description);
	return;
}

void
BudgetItem::set_account(Account const& p_account)
{
	impl().set_account(p_account);
	return;
}

void
BudgetItem::set_frequency(Frequency const& p_frequency)
{
	impl().set_frequency(p_frequency);
	return;
}

void
BudgetItem::set_amount(jewel::Decimal const& p_amount)
{
	impl().set_amount(p_amount);
	return;
}

wxString
BudgetItem::description() const
{
	return impl().description();
}

Account
BudgetItem::account() const
{
	return impl().account();
}

Frequency
BudgetItem::frequency() const
{
	return impl().frequency();
}

jewel::Decimal
BudgetItem::amount() const
{
	return impl().amount();
}

void
BudgetItem::mimic(BudgetItem const& rhs)
{
	impl().mimic(rhs.impl());
	return;
}

BudgetItem::BudgetItem(sqloxx::Handle<BudgetItemImpl> const& p_handle):
	PhatbooksPersistentObject(p_handle)
{
}

void
BudgetItem::output_budget_item_aux(ostream& os, BudgetItem const& bi)
{
	os << "BUDGET ITEM";
	if (bi.has_id())
	{
		// lexical cast to avoid unwanted formatting
		os << " ID " + lexical_cast<string>(bi.id());
	}
	os << ": "
	   << finformat_std8
	   	  	(	bi.amount(),
		  		BasicDecimalFormatFlags().set(string_flags::hard_align_right)
			)
	   << " ";
	os << frequency_description(bi.frequency(), "per");
	if (!bi.description().empty())
	{
		os << " (" << wx_to_std8(bi.description()) << ")";
	}
	return;
}

ostream&
operator<<(ostream& os, BudgetItem const& bi)
{
	jewel::output_aux(os, bi, BudgetItem::output_budget_item_aux);
	return os;
}

Decimal
normalized_total
(	vector<BudgetItem>::const_iterator b,
	vector<BudgetItem>::const_iterator const& e
)
{
	assert (e - b > 0);  // Assert precondition.
	PhatbooksDatabaseConnection& dbc = b->database_connection();
	Commodity commodity(dbc);
	// WARNING Temporary hack - if Accounts can ever have Commodities other
	// than the default Commodity, then this will no longer work.
	try
	{
		commodity = b->account().commodity();
	}
	catch (jewel::UninitializedOptionalException&)
	{
		commodity = dbc.default_commodity();
	}
	Decimal::places_type const prec = commodity.precision();
	Decimal ret(0, prec);
	for ( ; b != e; ++b)
	{
		assert
		(	b->database_connection().supports_budget_frequency
			(	b->frequency()
			)
		);
		ret += convert_to_canonical(b->frequency(), b->amount());
	}
	return round(convert_from_canonical(dbc.budget_frequency(), ret), prec);
}

}  // namespace phatbooks
