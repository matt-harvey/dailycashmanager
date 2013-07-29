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

using boost::optional;
using jewel::Decimal;
using jewel::value;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

PLReport::PLReport
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
		account_super_type::AccountSuperType const s_type =
			super_type(account.account_type());
		if (s_type != account_super_type::pl)
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
	// TODO HIGH PRIORITY Implement.
}


}  // namespace gui
}  // namespace phatbooks
