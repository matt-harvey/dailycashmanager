#include "account_dialog.hpp"
#include "account.hpp"
#include "frame.hpp"
#include <boost/noncopyable.hpp>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

AccountDialog::AccountDialog(Frame* p_parent, Account& p_account):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_account(p_account)
{
	// TODO Implement
}

}  // namespace gui
}  // namespace phatbooks
