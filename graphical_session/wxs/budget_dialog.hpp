#ifndef GUARD_budget_dialog_hpp
#define GUARD_budget_dialog_hpp

#include <boost/noncopyable.hpp>
#include <wx/combobox.h>

namespace phatbooks
{

// Begin forward declarations

class Account;

namespace gui
{

class Frame;

// End forward declarations


class BudgetDialog: public wxComboBox, private boost::noncopyable
{
public:

	BudgetDialog(Frame* p_parent, Account const& p_account);

private:

};  // class BudgetDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_budget_dialog_hpp
