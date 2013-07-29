#include "balance_sheet_report.hpp"
#include "account.hpp"
#include "entry_reader.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>

using boost::optional;
using jewel::Decimal;
using jewel::value;

namespace gregorian = boost::gregorian;

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
	ActualOrdinaryEntryReader reader(database_connection());	
	ActualOrdinaryEntryReader::const_iterator it = reader.begin();
	ActualOrdinaryEntryReader::const_iterator const end = reader.end();
	for ( ; it != end; ++it)
	{
		Account const account = it->account();
		Account::Id const account_id = account.id();
		BalanceMap::iterator jt = m_balance_map.find(account_id);
		if (jt == m_balance_map.end())
		{
			BalanceDatum const balance_datum(account);
			m_balance_map[account_id] = balance_datum;
		}
		else
		{
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
	}
	return;
}

void
BalanceSheetReport::display_text()
{
	// TODO Implement this.
	// Assume m_balance_map is up-to-date. Use its contents to display
	// the report contents.
	return;
}

BalanceSheetReport::~BalanceSheetReport()
{
}

}  // namespace gui
}  // namespace phatbooks
