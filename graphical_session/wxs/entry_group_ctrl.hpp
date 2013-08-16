// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_group_ctrl_hpp
#define GUARD_entry_group_ctrl_hpp

#include "account.hpp"
#include "account_type.hpp"
#include "decimal_text_ctrl.hpp"
#include "entry.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <vector>

namespace phatbooks
{


// Begin forward declarations

class Journal;
class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class TransactionCtrl;

// End forward declarations

/**
 * Widget for entering details regarding a single "side" of a transaction.
 * This could comprise multiple Entries. A "side" usually represents
 * either all the credit Entries of a Journal, or all the debit Entries.
 * A side is either "source" (usually credit), or "destination"
 * (usually debit). However, the "source" and "destination" concepts may
 * not always correspond to debit and credit.
 *
 * @todo Document better the concept of a "transaction" versus that of a
 * Journal.
 */
class EntryGroupCtrl: public wxPanel, private boost::noncopyable
{
public:

	/**
	 * Precondition: p_journal should have all the basic attributes of a
	 * ProtoJournal initialized. Also all the
	 * Entries in p_journal must be have all their attributes initialized,
	 * except they may or may not have an ID.
	 */
	EntryGroupCtrl
	(	TransactionCtrl* p_parent,
		wxSize const& p_text_ctrl_size,
		Journal const& p_journal,
		transaction_side::TransactionSide p_transaction_side,
		PhatbooksDatabaseConnection& p_database_connection
	);

	~EntryGroupCtrl();

	void refresh_for_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);

	jewel::Decimal primary_amount() const;

	jewel::Decimal total_amount() const;

	/**
	 * @returns a vector of newly created Entries (WITHOUT ids), based
	 * on the data currently in the EntryGroupCtrl. The Entries will not
	 * be associated with any Journal.
	 */
	std::vector<Entry> make_entries() const;

	bool is_all_zero() const;

	/**
	 * Go through each row. If that row corresponds to an entry which
	 * is reconciled, then disable it and set a tooltip explaining why it
	 * is disabled. Otherwise enable it and remove any tooltip. Returns
	 * \e true if at least one Entry is reconciled; otherwise, returns
	 * \e false.
	 */
	bool reflect_reconciliation_statuses();

private:

	class EntryDecimalTextCtrl: public DecimalTextCtrl
	{
	public:
		EntryDecimalTextCtrl(EntryGroupCtrl* p_parent, wxSize const& p_size);
		virtual ~EntryDecimalTextCtrl();
	private:
		void on_left_double_click(wxMouseEvent& event);
		DECLARE_EVENT_TABLE()
	};

	friend class EntryDecimalTextCtrl;

	void configure_available_account_types();
	void configure_top_row(bool p_include_split_button);
	void pop_row();
	void push_row
	(	Entry const& p_entry,
		boost::optional<jewel::Decimal> const& p_previous_row_amount,
		bool p_multiple_entries
	);
	void adjust_layout_for_new_number_of_rows();
	wxString side_description() const;

	void on_unsplit_button_click(wxCommandEvent& event);
	void on_split_button_click(wxCommandEvent& event);

	bool is_source() const;

	/**
	 * Adjusts the amount in \e p_target on the basis of
	 * such that the EntryGroupCtrl becomes balanced.
	 */
	void autobalance(EntryDecimalTextCtrl* p_target);

	size_t num_rows() const;

	PhatbooksDatabaseConnection& m_database_connection;

	static unsigned int const s_unsplit_button_id = wxID_HIGHEST + 1;
	static unsigned int const s_split_button_id = s_unsplit_button_id + 1;

	transaction_side::TransactionSide m_transaction_side;
	transaction_type::TransactionType m_transaction_type;
	std::vector<account_type::AccountType>* m_available_account_types;

	wxSize m_text_ctrl_size;

	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_side_descriptor;
	wxButton* m_unsplit_button;
	wxButton* m_split_button;

	friend struct EntryRow;

	// TODO HIGH PRIORITY Refactor EntryGroupCtrl to make use of this.
	struct EntryRow
	{
		EntryRow::EntryRow(Entry const& p_entry);
		AccountCtrl* account_ctrl;
		wxTextCtrl* comment_ctrl;
		EntryDecimalTextCtrl* amount_ctrl;
		Entry entry;
	};

	std::vector<EntryRow> m_entry_rows;

	size_t m_current_row;

	DECLARE_EVENT_TABLE()

};  // class EntryGroupCtrl


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_group_ctrl_hpp
