#include "balance_sheet_report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "entry_reader.hpp"
#include "locale.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/window.h>
#include <list>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using std::list;

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
	),
	m_next_row(0)
{
	assert (m_balance_map.empty());
}

void
BalanceSheetReport::do_generate()
{
	refresh_balance_map();
	display_text();

	// Don't do "FitInside()", "configure_scrollbars" or that "admin" stuff,
	// as this is done in the Report base class, in Report::generate().
	return;
}

void
BalanceSheetReport::refresh_balance_map()
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

	make_text(wxString("Opening balance"), 1);
	make_text(wxString("Movement"), 2);
	make_text(wxString("Closing balance"), 3);

	++m_next_row;

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
	// Bare scope - Do the asset section of the report
	{
		make_text(wxString("Assets"), 0);
		++m_next_row;
		list<wxString>::const_iterator it = asset_names.begin();
		list<wxString>::const_iterator const end = asset_names.end();
		for ( ; it != end; ++it)
		{
			make_text(*it, 0);
			Account const account(database_connection(), wx_to_bstring(*it));
			BalanceMap::const_iterator const jt =
				m_balance_map.find(account.id());
			assert (jt != m_balance_map.end());
			BalanceDatum const& datum = jt->second;
			Decimal const& ob = datum.opening_balance;
			Decimal const& cb = datum.closing_balance;
			make_number_text(ob, 1);
			make_number_text(cb - ob, 2);
			make_number_text(cb, 3);
			++m_next_row;
		}
	}

	// TODO Finish implementing this.
	return;
}

void
BalanceSheetReport::make_text(wxString const& p_text, int p_column)
{
	wxStaticText* header = new wxStaticText
	(	this,
		wxID_ANY,
		p_text,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	top_sizer().Add(header, wxGBPosition(m_next_row, p_column));
	return;
}

void
BalanceSheetReport::make_number_text(Decimal const& p_amount, int p_column)
{
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		finformat_wx(p_amount, locale()),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT
	);
	top_sizer().Add
	(	text,
		wxGBPosition(m_next_row, p_column),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
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
