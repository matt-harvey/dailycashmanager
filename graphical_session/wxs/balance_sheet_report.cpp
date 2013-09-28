// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "balance_sheet_report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "entry_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/window.h>
#include <list>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using std::list;
using std::vector;

namespace gregorian = boost::gregorian;

// For debugging
	#include <jewel/log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{
namespace gui
{

BalanceSheetReport::BalanceSheetReport
(	ReportPanel* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> p_maybe_min_date,
	optional<gregorian::date> p_maybe_max_date
):
	Report
	(	p_parent,
		p_size,
		p_database_connection,
		p_maybe_min_date,
		p_maybe_max_date
	)
{
	JEWEL_ASSERT (m_balance_map.empty());
}

void
BalanceSheetReport::do_generate()
{
	refresh_map();
	display_body();

	// Don't do "FitInside()", "configure_scrollbars" or that "admin" stuff,
	// as this is done in the Report base class, in Report::generate().
	return;
}

void
BalanceSheetReport::refresh_map()
{
	// TODO Can we just ignore equity Accounts here?
	m_balance_map.clear();
	JEWEL_ASSERT (m_balance_map.empty());
	optional<gregorian::date> const maybe_max_d = maybe_max_date();
	gregorian::date const min_d = min_date();

	// TODO We simply use the opening balance and current balance
	// of each Account, to optimize for the special but probably common
	// case where the min and max date are both blank.

	EntryTableIterator it =
		make_date_ordered_actual_ordinary_entry_table_iterator
		(	database_connection()
		);
	EntryTableIterator const end;	
	for ( ; it != end; ++it)
	{
		Account const account = it->account();
		AccountSuperType const s_type =
			super_type(account.account_type());
		if (s_type != AccountSuperType::balance_sheet)
		{
			continue;
		}
		Account::Id const account_id = account.id();
		BalanceMap::iterator jt = m_balance_map.find(account_id);
		if (jt == m_balance_map.end())
		{
			BalanceDatum const balance_datum(account);
			m_balance_map[account_id] = balance_datum;
			jt = m_balance_map.find(account_id);
		}
		JEWEL_ASSERT (jt != m_balance_map.end());
		gregorian::date const date = it->date();
		if (maybe_max_d && (date > value(maybe_max_d)))
		{
			break;
		}
		Decimal const amount = it->amount();
		jt->second.closing_balance += amount;
		if (date < min_d)
		{
			jt->second.opening_balance += amount;
		}
	}
	return;
}

void
BalanceSheetReport::display_body()
{
	// Assume m_balance_map is up-to-date. Use its contents to display
	// the report contents.
	
	// TODO Can we just ignore equity Accounts here?

	increment_row();

	display_text(wxString("Opening balance "), 2, wxALIGN_RIGHT);
	display_text(wxString("  Movement "), 3, wxALIGN_RIGHT);
	display_text(wxString("  Closing balance "), 4, wxALIGN_RIGHT);

	increment_row();
	
	list<wxString> asset_names;
	list<wxString> equity_names;
	list<wxString> liability_names;

	for (auto const& elem: m_balance_map)
	{
		Account const account(database_connection(), elem.first);
		wxString const name = account.name();
		switch (account.account_type())
		{
		case AccountType::asset:
			asset_names.push_back(name);
			break;
		case AccountType::liability:
			liability_names.push_back(name);
			break;
		case AccountType::equity:
			equity_names.push_back(name);
			break;
		default:
			JEWEL_HARD_ASSERT (false);
		}
	}
	asset_names.sort();
	equity_names.sort();
	liability_names.sort();

	vector<wxString> section_titles;
	section_titles.push_back(wxString("ASSETS"));
	// WARNING Assuming no Equity Account.
	section_titles.push_back(wxString("LIABILITIES"));
	vector<AccountType> section_account_types;
	section_account_types.push_back(AccountType::asset);
	// WARNING Assuming no Equity Account.
	section_account_types.push_back(AccountType::liability);
	JEWEL_ASSERT (section_titles.size() == section_account_types.size());

	Decimal const zero
	(	0,
		database_connection().default_commodity().precision()
	);
	Decimal net_assets_opening = zero;
	Decimal net_assets_closing = zero;
	for (vector<wxString>::size_type i = 0 ; i != section_titles.size(); ++i)
	{
		// WARNING This relies on every Account having the same Commodity.
		Decimal opening_balance_total = zero;
		Decimal closing_balance_total = zero;
		list<wxString>* names = 0;
		switch(section_account_types.at(i))
		{
		case AccountType::asset:
			names = &asset_names;
			break;
		case AccountType::liability:
			names = &liability_names;
			break;
		default:
			JEWEL_HARD_ASSERT (false);
		}
		display_text(section_titles.at(i), 1);
		
		increment_row();

		JEWEL_ASSERT (names);
		for (wxString const& name: *names)
		{
			Account const account(database_connection(), name);
			BalanceMap::const_iterator const jt =
				m_balance_map.find(account.id());
			JEWEL_ASSERT (jt != m_balance_map.end());
			BalanceDatum const& datum = jt->second;
			Decimal const& ob = datum.opening_balance;
			Decimal const& cb = datum.closing_balance;

			// Only show Accounts with non-zero balances
			if ((ob != zero) || (cb != zero))
			{
				display_text(name, 1);
				display_decimal(ob, 2);
				display_decimal(cb - ob, 3);
				display_decimal(cb, 4);
				opening_balance_total += ob;
				closing_balance_total += cb;

				increment_row();
			}
		}
		display_text(wxString("  Total"), 1);
		display_decimal(opening_balance_total, 2);
		display_decimal(closing_balance_total - opening_balance_total, 3);
		display_decimal(closing_balance_total, 4);
		net_assets_opening += opening_balance_total;
		net_assets_closing += closing_balance_total;

		increment_row();
		increment_row();
	}

	display_text(wxString("  Net assets"), 1);
	display_decimal(net_assets_opening, 2);
	display_decimal(net_assets_closing - net_assets_opening, 3);
	display_decimal(net_assets_closing, 4);

	increment_row();

	return;
}

BalanceSheetReport::BalanceDatum::BalanceDatum(Account const& p_account):
	opening_balance(0, p_account.commodity().precision()),
	closing_balance(0, p_account.commodity().precision())
{
}

}  // namespace gui
}  // namespace phatbooks
