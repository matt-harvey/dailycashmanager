/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_transaction_ctrl_hpp_6244141914064131
#define GUARD_transaction_ctrl_hpp_6244141914064131

#include "account.hpp"
#include "decimal_text_ctrl.hpp"
#include "gridded_scrolled_panel.hpp"
#include "persistent_journal.hpp"
#include "transaction_type_ctrl.hpp"
#include "transaction_type.hpp"
#include <jewel/decimal_fwd.hpp>
#include <jewel/on_windows.hpp>
#include <sqloxx/handle.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <memory>

namespace phatbooks
{

// Begin forward declarations

class DraftJournal;
class Entry;
class OrdinaryJournal;
class ProtoJournal;
class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class DateCtrl;
class EntryGroupCtrl;
class FrequencyCtrl;
class TopPanel;

// End forward declarations


/**
 * Dialog in which user creates a new transaction (i.e. Journal).
 *
 * @todo HIGH PRIORITY Ensure this can handle situation where insufficient
 * default Accounts are available.
 *
 * @todo HIGH PRIORITY When editing an existing transaction, if the user
 * changes the TransactionType, then the AccountCtrls are reset, and the user
 * loses the existing Account information. Prevent this from occurring.
 *
 * @todo MEDIUM PRIORITY It may not be obvious to the user that the
 * TransactionCtrl actually is for creating new transactions (rather than for,
 * say, filtering the information that is displayed to the left).
 *
 * @todo HIGH PRIORITY Boundary around comment boxes and date box looks
 * wrong under certain circumstances under Windows.
 */
class TransactionCtrl: public GriddedScrolledPanel
{
public:

	/**
	 * Create a TransactionCtrl to allow the user to edit an
	 * already-saved OrdinaryJournal.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<OrdinaryJournal> const& p_journal
	);

	/**
	 * Create a TransactionCtrl to allow the user to edit an
	 * already-saved DraftJournal.
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		wxSize const& p_size,
		sqloxx::Handle<DraftJournal> const& p_journal
	);

	/**
	 * Create a TransactionCtrl to allow the user to edit a
	 * ProtoJournal (which they will then be able to convert into
	 * a PersistentJournal and save).
	 */
	TransactionCtrl
	(	TopPanel* p_parent,
		wxSize const& p_size,
		ProtoJournal& p_journal,
		PhatbooksDatabaseConnection& p_database_connection
	);

	TransactionCtrl(TransactionCtrl const&) = delete;
	TransactionCtrl(TransactionCtrl&&) = delete;
	TransactionCtrl& operator=(TransactionCtrl const&) = delete;
	TransactionCtrl& operator=(TransactionCtrl&&) = delete;
	~TransactionCtrl() = default;

	/**
	 * Refresh the selections available in the AccountCtrls in the
	 * TransactionCtrl, and the selection in the TransactionTypeCtrl,
	 * to match p_transaction_type.
	 */
	void refresh_for_transaction_type
	(	TransactionType p_transaction_type
	);

	jewel::Decimal primary_amount() const;

	/**
	 * Update to reflect newly created Account.
	 */
	void update_for_new(sqloxx::Handle<Account> const& p_saved_object);

	/**
	 * Update to reflect Account having been amended (and the changes having
	 * been saved).
	 */
	void update_for_amended(sqloxx::Handle<Account> const& p_saved_object);

	/**
	 * Update to reflect possible change in reconciliation status
	 * of \e p_entry.
	 */
	void update_for_reconciliation_status
	(	sqloxx::Handle<Entry> const& p_entry
	);

	// Reset everything in TransactionCtrl to create and edit a new
	// ProtoJournal.
	void reset();

private:
	void on_cancel_button_click(wxCommandEvent& event);
	void on_delete_button_click(wxCommandEvent& event);
	void on_ok_button_click(wxCommandEvent& event);

	void reflect_reconciliation_statuses();

	// Adds some blank space to the right to allow space for vertical
	// scrollbar.
	void add_dummy_column();

	// Places size of "standard text box" in p_text_box_size.
	void configure_top_controls
	(	TransactionType p_transaction_type,
		wxSize& p_text_box_size,
		jewel::Decimal const& p_primary_amount,
		std::vector<TransactionType> const&
			p_available_transaction_types
	);

	// Clear all contents.
	void clear_all();

	// Assumes current contents are all clear.
	void configure_for_editing_proto_journal(ProtoJournal& p_journal);

	// Assume current contents are all clear except for m_journal, which
	// must have been initialized.
	void configure_for_editing_persistent_journal();

	/**
	 * @returns true if and only if journal was actually posted.
	 */
	bool post_journal();

	/**
	 * Removes *m_journal from database.
	 *
	 * @returns true if and only if journal was actually removed.
	 */
	bool remove_journal();

	/**
	 * @returns true if and only if existing journal was successfully saved.
	 */
	bool save_existing_journal();

	bool is_balanced() const;

	TransactionTypeCtrl* m_transaction_type_ctrl;
	EntryGroupCtrl* m_source_entry_ctrl;
	EntryGroupCtrl* m_destination_entry_ctrl;
	DecimalTextCtrl* m_primary_amount_ctrl;
	FrequencyCtrl* m_frequency_ctrl;
	DateCtrl* m_date_ctrl;
	wxButton* m_cancel_button;
	wxButton* m_delete_button;
	wxButton* m_ok_button;
	sqloxx::Handle<PersistentJournal> m_journal;

	static unsigned int const s_date_ctrl_id =
		wxID_HIGHEST + 1;
	static unsigned int const s_primary_amount_ctrl_id =
		s_date_ctrl_id + 1;
	static unsigned int const s_transaction_type_ctrl_id =
		s_primary_amount_ctrl_id + 1;
	static unsigned int const s_delete_button_id =
		s_transaction_type_ctrl_id + 1;
	static unsigned int const s_min_entry_row_id =
		s_delete_button_id + 1;

	DECLARE_EVENT_TABLE()

};  // class TransactionCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_ctrl_hpp_6244141914064131
