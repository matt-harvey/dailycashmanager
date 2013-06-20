#ifndef GUARD_entry_ctrl_hpp
#define GUARD_entry_ctrl_hpp


#include "account.hpp"
#include "account_reader.hpp"
#include "transaction_type.hpp"
#include <boost/noncopyable.hpp>
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
class TransactionCtrl;

// End forward declarations


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
		bool p_is_source
	);

	void refresh_for_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);

	~EntryCtrl();

private:
	
	wxString side_description() const;

	void on_split_button_click(wxCommandEvent& event);
	void add_row();

	PhatbooksDatabaseConnection& m_database_connection;

	static unsigned int const s_split_button_id = wxID_HIGHEST + 1;

	bool m_is_source;
	transaction_type::TransactionType m_transaction_type;
	AccountReaderBase* m_account_reader;

	wxSize m_text_ctrl_size;

	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_side_descriptor;
	std::vector<AccountCtrl*> m_account_name_boxes;
	std::vector<wxTextCtrl*> m_comment_boxes;
	std::vector<wxButton*> m_split_buttons;

	size_t m_next_row;

	DECLARE_EVENT_TABLE()

};  // class EntryCtrl


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_entry_ctrl_hpp
