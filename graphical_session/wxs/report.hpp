#ifndef GUARD_report_hpp
#define GUARD_report_hpp

#include "account_type.hpp"
#include "entry.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class OrdinaryJournal;
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

	void update_for_new(OrdinaryJournal const& p_journal);
	void update_for_amended(OrdinaryJournal const& p_journal);
	void update_for_new(Account const& p_account);
	void update_for_amended(Account const& p_account);
	void update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids);

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
