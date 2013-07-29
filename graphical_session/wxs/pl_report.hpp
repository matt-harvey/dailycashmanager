#ifndef GUARD_pl_report_hpp
#define GUARD_pl_report_hpp

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

class PLReport: public Report
{
public:
	PLReport
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> p_maybe_min_date,
		boost::optional<boost::gregorian::date> p_maybe_max_date
	);

	virtual ~PLReport();

private:
	virtual void do_generate();

	void refresh_map();

	void display_text();

};  // class PLReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_pl_report_hpp
