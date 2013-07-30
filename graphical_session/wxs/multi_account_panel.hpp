#ifndef GUARD_multi_account_panel_hpp
#define GUARD_multi_account_panel_hpp

#include <boost/noncopyable.hpp>
#include <wx/scrolwin.h>

namespace phatbooks
{
namespace gui
{

/**
 * An instance of this class is to be included in the
 * SetupWizard::BalanceSheetAccountPage. This class represents a scrolled
 * panel in which the user can create and configure multiple Accounts,
 * with the types, descriptions and opening balances.
 */
class MultiAccountPanel: public wxScrolledWindow, private boost::noncopyable
{
public:
	virtual ~MultiAccountPanel();

protected:

private:

};  // class MultiAccountPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_multi_account_panel_hpp
