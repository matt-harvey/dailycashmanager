// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_report_hpp
#define GUARD_report_hpp

#include "account_type.hpp"
#include "entry.hpp"
#include "gridded_scrolled_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/gdicmn.h>
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
 *
 * @todo The "update_for_" functions deliberately do nothing. The user must
 * re-run the Report explicitly for it to be updated after a Journal or
 * Account is created, amended or deleted. Is this the behaviour we want?
 * It is inconsistent with what is currently occurring in the
 * EntryListPanel and the ReconciliationPanel.
 */
class Report: public GriddedScrolledPanel
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

private:
	virtual void do_generate() = 0;
	boost::gregorian::date m_min_date;
	boost::optional<boost::gregorian::date> m_maybe_max_date;

};  // class Report

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_hpp
