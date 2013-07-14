#ifndef GUARD_budget_dialog_hpp
#define GUARD_budget_dialog_hpp

#include "budget_item.hpp"
#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class Account;
class PhatbooksDatabaseConnection;

namespace gui
{

class Frame;

// End forward declarations


class BudgetDialog: public wxDialog, private boost::noncopyable
{
public:

	/**
	 * @param p_parent parent window
	 * @param p_account must have id.
	 */
	BudgetDialog(Frame* p_parent, Account const& p_account);

private:

	void reset_budget_summary();
	void push_item(BudgetItem const& p_budget_item);
	PhatbooksDatabaseConnection& database_connection();

	static int const s_pop_item_button_id = wxID_HIGHEST + 1;
	static int const s_push_item_button_id = s_pop_item_button_id + 1;

	wxString generate_summary_amount_text();
	wxString generate_summary_frequency_text();

	size_t m_next_row;

	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_summary_amount_text;
	wxStaticText* m_summary_frequency_text;
	wxButton* m_pop_item_button;
	wxButton* m_push_item_button;
	wxButton* m_cancel_button;
	wxButton* m_ok_button;

	Account const& m_account;
	std::vector<BudgetItem> m_budget_items;

};  // class BudgetDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_budget_dialog_hpp
