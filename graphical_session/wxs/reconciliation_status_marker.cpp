#include "reconciliation_status_marker.hpp"
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

wxString
reconciliation_status_marker(bool p_is_reconciled)
{
	// TODO HIGH PRIORITY Check that the "thick check" mark
	// displays properly on Windows.
	return p_is_reconciled? wxString("\u2714"): wxString();
}


}  // namespace gui
}  // namespace phatbooks
