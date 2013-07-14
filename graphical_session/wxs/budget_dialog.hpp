#ifndef GUARD_budget_dialog_hpp
#define GUARD_budget_dialog_hpp

#include <boost/noncopyable.hpp>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>

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

	/**
	 * @todo Implement.
	 */
	void reset_budget_summary();

	PhatbooksDatabaseConnection& database_connection();

	wxString generate_summary_amount_text();
	wxString generate_summary_frequency_text();

	wxGridBagSizer* m_top_sizer;
	wxStaticText* m_summary_amount_text;
	wxStaticText* m_summary_frequency_text;
	wxButton* m_cancel_button;
	wxButton* m_ok_button;

	Account const& m_account;

};  // class BudgetDialog


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_budget_dialog_hpp
