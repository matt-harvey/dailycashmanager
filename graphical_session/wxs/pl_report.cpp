#include "pl_report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <list>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using std::list;
using std::vector;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

PLReport::PLReport
(	ReportPanel* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
):
	Report
	(	p_parent,
		p_size,
		p_database_connection,
		p_maybe_min_date,
		p_maybe_max_date
	)
{
}

PLReport::~PLReport()
{
}

void
PLReport::do_generate()
{
	refresh_map();
	display_text();

	// Don't do "FitInside()", "configure_scrollbars" or that "admin" stuff,
	// as this is done in the Report base class, in Report::generate().
	return;
}

void
PLReport::refresh_map()
{
	// TODO There is a lot of duplicated code between here and
	// BalanceSheetReport::refresh_map().
	m_map.clear();
	assert (m_map.empty());
	optional<gregorian::date> const maybe_max_d = maybe_max_date();
	gregorian::date const min_d = min_date();
	Decimal const zero
	(	0,
		database_connection().default_commodity().precision()
	);
	typedef ActualOrdinaryEntryReader ReaderT;
	ReaderT const reader(database_connection());
	ReaderT::const_iterator it = reader.begin();
	ReaderT::const_iterator const end = reader.end();
	for ( ; it != end; ++it)
	{
		Account const account = it->account();
		account_type::AccountType const atype = account.account_type();
		if
		(	(atype != account_type::revenue) &&
			(atype != account_type::expense)
		)
		{
			continue;
		}
		Account::Id const account_id = account.id();
		Map::iterator jt = m_map.find(account_id);
		if (jt == m_map.end())
		{
			assert
			(	database_connection().default_commodity() ==
				account.commodity()
			);
			m_map[account_id] = zero;
			jt = m_map.find(account_id);
		}
		assert (jt != m_map.end());
		gregorian::date const date = it->date();
		if (maybe_max_d && (date > value(maybe_max_d)))
		{
			break;
		}
		if (date >= min_d)
		{
			jt->second += it->amount();
		}
	}
	return;
}

void
PLReport::display_text()
{
	// Assume m_map is up-to-date. Use it contents to display
	// the report contents.

	// TODO We should show weekly and/or daily averages as well.

	increment_row();

	make_text(wxString("Total "), 1, wxALIGN_RIGHT);

	increment_row();

	list<wxString> revenue_names;
	list<wxString> expense_names;
	list<wxString> pure_envelope_names;

	// Bare scope
	{
		Map::const_iterator it = m_map.begin();
		Map::const_iterator const end = m_map.end();
		for ( ; it != end; ++it)
		{
			Account const account(database_connection(), it->first);
			wxString const name = bstring_to_wx(account.name());
			switch (account.account_type())
			{
			case account_type::revenue:
				revenue_names.push_back(name);
				break;
			case account_type::expense:
				expense_names.push_back(name);
				break;
			default:
				assert (false);
			}
		}
		revenue_names.sort();
		expense_names.sort();
	}
	vector<wxString> section_titles;
	section_titles.push_back(wxString("REVENUE"));
	section_titles.push_back(wxString("EXPENSE"));
	vector<account_type::AccountType> section_account_types;
	section_account_types.push_back(account_type::revenue);
	section_account_types.push_back(account_type::expense);
	assert (section_titles.size() == section_account_types.size());

	Decimal const zero
	(	0,
		database_connection().default_commodity().precision()
	);
	Decimal net_revenue = zero;
	for (vector<wxString>::size_type i = 0; i != section_titles.size(); ++i)
	{
		// WARNING This relies on every Account having the same Commodity
		Decimal total = zero;
		list<wxString>* names = 0;
		account_type::AccountType const account_type =
			section_account_types.at(i);
		switch (account_type)
		{
		case account_type::revenue:
			names = &revenue_names;
			break;
		case account_type::expense:
			names = &expense_names;
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
			Map::const_iterator const jt = m_map.find(account.id());
			assert (jt != m_map.end());
			Decimal const& b =
			(	(account_type == account_type::expense)?
				jt->second:
				-(jt->second)
			);

			// Only show Accounts with non-zero balances
			if (b != zero)
			{
				make_text(*it, 0);
				make_number_text(b, 1);
				total += b;

				increment_row();
			}
		}
		make_text(wxString("  Total"), 0);
		make_number_text(total, 1);
		net_revenue +=
		(	(account_type == account_type::revenue)?
			total:
			-total
		);

		increment_row();
		increment_row();
	}

	make_text(wxString("  Net revenue"), 0);
	make_number_text(net_revenue, 1);

	increment_row();
	
	return;
}


}  // namespace gui
}  // namespace phatbooks
