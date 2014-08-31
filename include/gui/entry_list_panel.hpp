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

#ifndef GUARD_entry_list_panel_hpp_3556466034407013
#define GUARD_entry_list_panel_hpp_3556466034407013

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/window.h>
#include <vector>

namespace dcm
{

// Begin forward declarations

class Account;
class Entry;
class OrdinaryJournal;
class DcmDatabaseConnection;

namespace gui
{

class AccountCtrl;
class Button;
class DateCtrl;
class EntryListCtrl;

// End forward declarations

/**
 * A panel consisting of an EntryListCtrl at the bottom, and widgets
 * at the top to enable the user to filter the displayed Entries for a
 * particular Account and/or date range.
 *
 * @todo MEDIUM PRIORITY Do we want the user to be able to view non-actual
 * transactions? If we do, then, among other things, the AccountCtrl will need
 * to allow the user to select Accounts of AccountType::pure_envelope.
 */
class EntryListPanel: public wxScrolledWindow
{
public:
	EntryListPanel
	(	wxWindow* p_parent,
		DcmDatabaseConnection& p_database_connection,
		bool p_support_reconciliations = false
	);

	EntryListPanel(EntryListPanel const&) = delete;
	EntryListPanel(EntryListPanel&&) = delete;
	EntryListPanel& operator=(EntryListPanel const&) = delete;
	EntryListPanel& operator=(EntryListPanel&&) = delete;
	virtual ~EntryListPanel(); 

	void update_for_new(sqloxx::Handle<OrdinaryJournal> const& p_journal);
	void update_for_amended(sqloxx::Handle<OrdinaryJournal> const& p_journal);
	void update_for_new(sqloxx::Handle<Account> const& p_account);
	void update_for_amended(sqloxx::Handle<Account> const& p_account);
	void update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids);

	std::vector<sqloxx::Handle<Entry> > selected_entries();

	// TODO LOW PRIORITY This should really be private, but we need to call it
	// from TopPanel to ensure EntryListCtrl is properly sized, AFTER the
	// EntryListPanel has been constructed. Make this nicer.
	void configure_entry_list_ctrl();

	void postconfigure_summary();

private:
	void preconfigure_summary();

	void on_refresh_button_click(wxCommandEvent& event);
	sqloxx::Handle<Account> selected_account() const;
	boost::optional<boost::gregorian::date> selected_min_date() const;
	boost::optional<boost::gregorian::date> selected_max_date() const;
	
	static int const s_account_ctrl_id = wxID_HIGHEST + 1;
	static int const s_min_date_ctrl_id = s_account_ctrl_id + 1;
	static int const s_max_date_ctrl_id = s_min_date_ctrl_id + 1;
	static int const s_refresh_button_id = s_max_date_ctrl_id + 1;
	static int const s_entry_list_ctrl_id = s_refresh_button_id + 1;

	bool m_support_reconciliations;
	int m_next_row;
	int m_client_size_aux;
	int m_text_ctrl_height;

	wxGridBagSizer* m_top_sizer;
	AccountCtrl* m_account_ctrl;
	DateCtrl* m_min_date_ctrl;
	DateCtrl* m_max_date_ctrl;
	Button* m_refresh_button;
	wxStaticText* m_reconciliation_hint;
	EntryListCtrl* m_entry_list_ctrl;
	std::vector<wxStaticText*> m_summary_label_text_items;
	std::vector<wxStaticText*> m_summary_data_text_items;
	DcmDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class EntryListPanel

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_entry_list_panel_hpp_3556466034407013
