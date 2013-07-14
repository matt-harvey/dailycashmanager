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
	void on_pop_item_button_click(wxCommandEvent& event);
	void on_push_item_button_click(wxCommandEvent& event);
	void on_ok_button_click(wxCommandEvent& event);
	void on_cancel_button_click(wxCommandEvent& event);

	/**
	 * Resets budget summary text at top of Dialog, on the basis
	 * of the budgets currently saved in the \e database, regardless of
	 * what is currently shown in the BudgetDialog itself.
	 *
	 * @todo Is this actually called anywhere?
	 */
	void reset_budget_summary();

	/**
	 * Updates the BudgetItems for m_account based on the data entered
	 * by the user.
	 *
	 * @returns true if an only if the BudgetItems for m_account are
	 * successfully updated and saved.
	 */
	bool update_budgets_from_dialog();

	void push_item(BudgetItem const& p_budget_item);
	PhatbooksDatabaseConnection& database_connection();

	static int const s_pop_item_button_id = wxID_HIGHEST + 1;
	static int const s_push_item_button_id = s_pop_item_button_id + 1;

	/**
	 * @returns string describing the standardized budget frequency for
	 * database_connection() on the basis of what is currently saved in the
	 * \e database, regardless of what is currently shown in the BudgetDialog
	 * itself.
	 */
	wxString generate_summary_amount_text();

	/**
	 * @returns string describing the budget amount for m_account on the basis
	 * of what is currently saved in the \e database, regardless of what
	 * is currently shown in the BudgetDialog itself.
	 */
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

	DECLARE_EVENT_TABLE()

};  // class BudgetDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_budget_dialog_hpp
