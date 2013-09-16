#include "reconciliation_status_marker.hpp"
#include <jewel/on_windows.hpp>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

wxString
reconciliation_status_marker(bool p_is_reconciled)
{
#ifdef JEWEL_ON_WINDOWS
	// TODO Provide a nicer looking symbol for use on Windows. We should
	// probably use a graphic rather than a string if we can, and then the
	// same graphic should work on all platforms.
	static const wxString marker("Y");
#else
	// TODO HIGH PRIORITY Does this work on Mac? What about Linuxes other than
	// Gnome? Can we test this in a more reliable way rather than
	// just assuming that it works on everything except Windows?
	static const wxString marker("\u2714");  // "thick check" symbol
#endif
	return p_is_reconciled? marker: wxString();
}


}  // namespace gui
}  // namespace phatbooks
