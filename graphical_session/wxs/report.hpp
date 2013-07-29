#ifndef GUARD_report_hpp
#define GUARD_report_hpp

#include "account_type.hpp"
#include "entry.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <wx/gbsizer.h>
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

	void generate();

protected:
	PhatbooksDatabaseConnection& database_connection();
	Report
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	// TODO There is code here surrounding min and max dates that is shared
	// with FilteredEntryListCtrl. We could factor this out using a protected
	// base class.
	boost::gregorian::date min_date() const;
	boost::optional<boost::gregorian::date> maybe_max_date() const;

	wxGridBagSizer& top_sizer();

	void increment_row();
	int next_row() const;

	void make_text
	(	wxString const& p_text,
		int p_column,
		int p_flags = wxALIGN_LEFT
	);

	void make_number_text(jewel::Decimal const& p_amount, int p_column);

private:
	virtual void do_generate() = 0;

	void configure_scrollbars();


	int m_next_row;
	wxGridBagSizer* m_top_sizer;
	PhatbooksDatabaseConnection& m_database_connection;
	boost::gregorian::date m_min_date;
	boost::optional<boost::gregorian::date> m_maybe_max_date;

};  // class Report

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_hpp
