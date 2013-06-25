// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_ctrl_hpp
#define GUARD_entry_ctrl_hpp


#include "account.hpp"
#include "account_reader.hpp"
#include "entry.hpp"
#include "transaction_type.hpp"
#include <boost/noncopyable.hpp>
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

class PhatbooksDatabaseConnection;

namespace gui
{

class AccountCtrl;
class DecimalTextCtrl;
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
 *
 * @todo Make the amount controls read-only unless and until additional
 * lines are added. We might even want to hide them entirely.
 */
class EntryCtrl:
	public wxPanel,
	private boost::noncopyable
{
public:
	EntryCtrl
	(	wxWindow* p_parent,
		std::vector<Account> const& p_accounts,
		PhatbooksDatabaseConnection& p_database_connection,
		transaction_type::TransactionType p_transaction_type,
		wxSize const& p_text_ctrl_size,
		bool p_is_source,
		jewel::Decimal const& p_primary_amount
	);

	~EntryCtrl();

	void refresh_for_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);

	void set_primary_amount(jewel::Decimal const& p_primary_amount);

	jewel::Decimal primary_amount() const;

	bool is_balanced() const;

	/**
	 * @returns a vector of newly created Entries (WITHOUT ids), based
	 * on the data currently in the EntryCtrl. The Entries will not
	 * be associated with any Journal.
	 */
	std::vector<Entry> make_entries() const;

	bool is_all_zero() const;

private:

	wxString side_description() const;

	void on_split_button_click(wxCommandEvent& event);
	void add_row();

	PhatbooksDatabaseConnection& m_database_connection;

	static unsigned int const s_split_button_id = wxID_HIGHEST + 1;

	bool m_is_source;
	jewel::Decimal m_primary_amount;
	transaction_type::TransactionType m_transaction_type;
	AccountReaderBase* m_account_reader;

	wxSize m_text_ctrl_size;

	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_side_descriptor;
	wxButton* m_split_button;
	std::vector<AccountCtrl*> m_account_name_boxes;
	std::vector<wxTextCtrl*> m_comment_boxes;
	std::vector<DecimalTextCtrl*> m_amount_boxes;

	size_t m_next_row;

	DECLARE_EVENT_TABLE()

};  // class EntryCtrl


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_ctrl_hpp
