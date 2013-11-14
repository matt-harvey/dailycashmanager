/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_report_hpp_0032136221431259167
#define GUARD_report_hpp_0032136221431259167

#include "account_type.hpp"
#include "gridded_scrolled_panel.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
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
 * @todo MEDIUM PRIORITY The "update_for_" functions deliberately do nothing.
 * The user must
 * re-run the Report explicitly for it to be updated after a Journal or
 * Account is created, amended or deleted. Is this the behaviour we want?
 * It is inconsistent with what is currently occurring in the
 * EntryListPanel and the ReconciliationPanel.
 *
 * @todo MEDIUM PRIORITY The report is rather plain looking. Make it look
 * nicer.
 */
class Report: public GriddedScrolledPanel
{
public:

	// Constructor is private - client code should user static "create"
	// function.
	
	Report(Report const&) = delete;
	Report(Report&&) = delete;
	Report& operator=(Report const&) = delete;
	Report& operator=(Report&&) = delete;
	virtual ~Report();

	/**
	 * @returns a pointer to a heap-allocated Report. Caller will be
	 * responsible for memory.
	 */
	static Report* create
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		AccountSuperType p_account_super_type,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date =
			boost::optional<boost::gregorian::date>(),
		boost::optional<boost::gregorian::date> const& p_maybe_max_date =
			boost::optional<boost::gregorian::date>()
	);

	void update_for_new(sqloxx::Handle<OrdinaryJournal> const& p_journal);
	void update_for_amended(sqloxx::Handle<OrdinaryJournal> const& p_journal);
	void update_for_new(sqloxx::Handle<Account> const& p_account);
	void update_for_amended(sqloxx::Handle<Account> const& p_account);
	void update_for_amended_budget(sqloxx::Handle<Account> const& p_account);
	void update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids);

	void generate();

protected:
	Report
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	boost::gregorian::date min_date() const;
	boost::optional<boost::gregorian::date> maybe_max_date() const;

private:
	virtual void do_generate() = 0;
	boost::gregorian::date m_min_date;
	boost::optional<boost::gregorian::date> m_maybe_max_date;

};  // class Report


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_hpp_0032136221431259167
