#include "balance_sheet_report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>
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
	#include <jewel/debug_log.hpp>
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
	assert (m_balance_map.empty());
}

void
BalanceSheetReport::do_generate()
{
	refresh_map();
	display_text();

	// Don't do "FitInside()", "configure_scrollbars" or that "admin" stuff,
	// as this is done in the Report base class, in Report::generate().
	return;
}

void
BalanceSheetReport::refresh_map()
{
	m_balance_map.clear();
	assert (m_balance_map.empty());
	optional<gregorian::date> const maybe_max_d = maybe_max_date();
	gregorian::date const min_d = min_date();
	typedef ActualOrdinaryEntryReader ReaderT;
	ReaderT reader(database_connection());	
	ReaderT::const_iterator it = reader.begin();
	ReaderT::const_iterator const end = reader.end();
	for ( ; it != end; ++it)
	{
		Account const account = it->account();
		account_super_type::AccountSuperType const s_type =
			super_type(account.account_type());
		if (s_type != account_super_type::balance_sheet)
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
		assert (jt != m_balance_map.end());
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
BalanceSheetReport::display_text()
{
	// Assume m_balance_map is up-to-date. Use its contents to display
	// the report contents.

	increment_row();

	make_text(wxString("Opening balance "), 1, wxALIGN_RIGHT);
	make_text(wxString("  Movement "), 2, wxALIGN_RIGHT);
	make_text(wxString("  Closing balance "), 3, wxALIGN_RIGHT);

	increment_row();
	
	list<wxString> asset_names;
	list<wxString> equity_names;
	list<wxString> liability_names;

	// Bare scope
	{
		BalanceMap::const_iterator it = m_balance_map.begin();
		BalanceMap::const_iterator const end = m_balance_map.end();
		for ( ; it != end; ++it)
		{
			Account const account(database_connection(), it->first);
			wxString const name = bstring_to_wx(account.name());
			switch (account.account_type())
			{
			case account_type::asset:
				asset_names.push_back(name);
				break;
			case account_type::liability:
				liability_names.push_back(name);
				break;
			case account_type::equity:
				equity_names.push_back(name);
				break;
			default:
				assert (false);
			}
		}
		asset_names.sort();
		equity_names.sort();
		liability_names.sort();
	}
	vector<wxString> section_titles;
	section_titles.push_back(wxString("ASSETS"));
	// WARNING Assuming no Equity Account.
	section_titles.push_back(wxString("LIABILITIES"));
	vector<account_type::AccountType> section_account_types;
	section_account_types.push_back(account_type::asset);
	// WARNING Assuming no Equity Account.
	section_account_types.push_back(account_type::liability);
	assert (section_titles.size() == section_account_types.size());

	Decimal const zero
	(	0,
		database_connection().default_commodity().precision()
	);
	Decimal net_assets_opening = zero;
	Decimal net_assets_closing = zero;

	vector<wxString>::size_type i = 0;
	vector<wxString>::size_type const sz = section_titles.size();
	for ( ; i != sz; ++i)
	{
		// WARNING This relies on every Account having the same Commodity.
		Decimal opening_balance_total = zero;
		Decimal closing_balance_total = zero;
		list<wxString>* names = 0;
		switch(section_account_types.at(i))
		{
		case account_type::asset:
			names = &asset_names;
			break;
		case account_type::liability:
			names = &liability_names;
			break;
		default:
			assert (false);
		}
		make_text(section_titles.at(i), 0);
		
		increment_row();

		list<wxString>::const_iterator it = names->begin();
		list<wxString>::const_iterator const end = names->end();
		for ( ; it != end; ++it)
		{
			Account const account(database_connection(), wx_to_bstring(*it));
			BalanceMap::const_iterator const jt =
				m_balance_map.find(account.id());
			assert (jt != m_balance_map.end());
			BalanceDatum const& datum = jt->second;
			Decimal const& ob = datum.opening_balance;
			Decimal const& cb = datum.closing_balance;

			// Only show Accounts with non-zero balances
			if ((ob != zero) || (cb != zero))
			{
				make_text(*it, 0);
				make_number_text(ob, 1);
				make_number_text(cb - ob, 2);
				make_number_text(cb, 3);
				opening_balance_total += ob;
				closing_balance_total += cb;

				increment_row();
			}
		}
		make_text(wxString("  Total"), 0);
		make_number_text(opening_balance_total, 1);
		make_number_text(closing_balance_total - opening_balance_total, 2);
		make_number_text(closing_balance_total, 3);
		net_assets_opening += opening_balance_total;
		net_assets_closing += closing_balance_total;

		increment_row();
		increment_row();
	}

	make_text(wxString("  Net assets"), 0);
	make_number_text(net_assets_opening, 1);
	make_number_text(net_assets_closing - net_assets_opening, 2);
	make_number_text(net_assets_closing, 3);

	increment_row();

	return;
}

BalanceSheetReport::~BalanceSheetReport()
{
}

BalanceSheetReport::BalanceDatum::BalanceDatum():
	opening_balance(0, 0),
	closing_balance(0, 0)
{
}

BalanceSheetReport::BalanceDatum::BalanceDatum(Account const& p_account):
	opening_balance(0, p_account.commodity().precision()),
	closing_balance(0, p_account.commodity().precision())
{
}

}  // namespace gui
}  // namespace phatbooks
