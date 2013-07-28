#ifndef GUARD_balance_sheet_report_hpp
#define GUARD_balance_sheet_report_hpp

#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class ReportPanel;

// end forward declarations

class BalanceSheetReport: public Report
{
public:
	BalanceSheetReport
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> p_maybe_min_date,
		boost::optional<boost::gregorian::date> p_maybe_max_date
	);

	virtual ~BalanceSheetReport();

private:

};  // class BalanceSheetReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_balance_sheet_report_hpp
