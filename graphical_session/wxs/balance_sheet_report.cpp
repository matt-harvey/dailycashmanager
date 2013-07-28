#include "balance_sheet_report.hpp"
#include "phatbooks_database_connection.hpp"
#include "report.hpp"
#include "report_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>

using boost::optional;

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
}

BalanceSheetReport::~BalanceSheetReport()
{
}

}  // namespace gui
}  // namespace phatbooks
