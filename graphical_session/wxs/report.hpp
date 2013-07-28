#ifndef GUARD_report_hpp
#define GUARD_report_hpp

#include "account_type.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class ReportPanel;

// End forward declarations

/**
 * Displays data constituting either a balance sheet report, or a statement
 * of income and expenses.
 */
class Report: public wxScrolledWindow, private boost::noncopyable
{
public:
	virtual ~Report();

	static Report* create
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		account_super_type::AccountSuperType p_account_super_type,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> const& p_maybe_max_date =
			boost::optional<boost::gregorian::date>()
	);

protected:
	PhatbooksDatabaseConnection& database_connection();

	Report
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

private:
	PhatbooksDatabaseConnection& m_database_connection;
	boost::optional<boost::gregorian::date> m_maybe_min_date;
	boost::optional<boost::gregorian::date> m_maybe_max_date;

};  // class Report

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_hpp
