// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "pl_report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "date.hpp"
#include "entry_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <list>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::SQLStatement;
using std::unique_ptr;
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

void
PLReport::do_generate()
{
	refresh_map();
	display_body();

	// Don't do "FitInside()", "configure_scrollbars" or that "admin" stuff,
	// as this is done in the Report base class, in Report::generate().
	return;
}

optional<int>
PLReport::maybe_num_days_in_period() const
{
	optional<int> ret;
	optional<gregorian::date> const maybe_max = maybe_max_date();
	if (maybe_max)
	{
		gregorian::date_duration const interval =
			value(maybe_max) - min_date();
		int const temp = interval.days() + 1;
		ret = ((temp >= 0)? temp: 0);
	}
	return ret;
}

void
PLReport::display_mean
(	int p_column,
	Decimal const& p_total,
	int p_count
)
{
	if (p_count == 0)
	{
		display_text(wxString("N/A "), p_column, wxALIGN_RIGHT);
	}
	else
	{
		display_decimal
		(	round(p_total / Decimal(p_count, 0), p_total.places()),
			p_column
		);
	}
	return;
}

void
PLReport::refresh_map()
{
	// TODO There is a lot of duplicated code between here and
	// BalanceSheetReport::refresh_map().
	m_map.clear();
	JEWEL_ASSERT (m_map.empty());
	optional<gregorian::date> const maybe_max_d = maybe_max_date();
	gregorian::date const min_d = min_date();
	Decimal const zero
	(	0,
		database_connection().default_commodity().precision()
	);

	unique_ptr<SQLStatement> statement =
		create_date_ordered_actual_ordinary_entry_selector
		(	database_connection(),
			min_date(),
			maybe_max_date()
		);
	while (statement->step())
	{
		Entry const entry
		(	database_connection(),
			statement->extract<Entry::Id>(0)
		);
		Account const account = entry.account();
		AccountType const atype = account.account_type();
		if
		(	(atype != AccountType::revenue) &&
			(atype != AccountType::expense)
		)
		{
			continue;
		}
		Account::Id const account_id = account.id();
		Map::iterator jt = m_map.find(account_id);
		if (jt == m_map.end())
		{
			JEWEL_ASSERT
			(	database_connection().default_commodity() ==
				account.commodity()
			);
			m_map[account_id] = zero;
			jt = m_map.find(account_id);
		}
		JEWEL_ASSERT (jt != m_map.end());
		gregorian::date const date = entry.date();
		JEWEL_ASSERT (!maybe_max_d || (date > value(maybe_max_d)))
		JEWEL_ASSERT (date >= min_d);
		jt->second += entry.amount();
	}
	return;
}

void
PLReport::display_body()
{
	// Assume m_map is up-to-date. Use it contents to display
	// the report contents.
	
	optional<int> const maybe_num_days = maybe_num_days_in_period();
	int const count_for_mean = (maybe_num_days? value(maybe_num_days): 0);

	increment_row();

	display_text(wxString("Total "), 2, wxALIGN_RIGHT);
	display_text(wxString("  Daily average "), 3, wxALIGN_RIGHT);

	increment_row();

	list<wxString> revenue_names;
	list<wxString> expense_names;
	list<wxString> pure_envelope_names;

	// Bare scope
	{
		for (auto const& elem: m_map)
		{
			Account const account(database_connection(), elem.first);
			wxString const name = account.name();
			switch (account.account_type())
			{
			case AccountType::revenue:
				revenue_names.push_back(name);
				break;
			case AccountType::expense:
				expense_names.push_back(name);
				break;
			default:
				JEWEL_HARD_ASSERT (false);
			}
		}
		revenue_names.sort();
		expense_names.sort();
	}
	vector<wxString> section_titles;
	section_titles.push_back(wxString("REVENUE"));
	section_titles.push_back(wxString("EXPENSE"));
	vector<AccountType> section_account_types;
	section_account_types.push_back(AccountType::revenue);
	section_account_types.push_back(AccountType::expense);
	JEWEL_ASSERT (section_titles.size() == section_account_types.size());

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
		AccountType const account_type =
			section_account_types.at(i);
		switch (account_type)
		{
		case AccountType::revenue:
			names = &revenue_names;
			break;
		case AccountType::expense:
			names = &expense_names;
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
			Map::const_iterator const jt = m_map.find(account.id());
			JEWEL_ASSERT (jt != m_map.end());
			Decimal const& b =
			(	(account_type == AccountType::expense)?
				jt->second:
				-(jt->second)
			);

			// Only show Accounts with non-zero balances
			if (b != zero)
			{
				display_text(name, 1);
				display_decimal(b, 2);
				total += b;

				display_mean(3, b, count_for_mean);

				increment_row();
			}
		}
		display_text(wxString("  Total"), 1);
		display_decimal(total, 2);

		display_mean(3, total, count_for_mean);		
		
		net_revenue +=
		(	(account_type == AccountType::revenue)?
			total:
			-total
		);

		increment_row();
		increment_row();
	}

	display_text(wxString("  Net revenue"), 1);
	display_decimal(net_revenue, 2);

	display_mean(3, net_revenue, count_for_mean);

	increment_row();
	
	return;
}


}  // namespace gui
}  // namespace phatbooks
