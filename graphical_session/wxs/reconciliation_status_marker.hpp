#ifndef GUARD_reconciliation_status_marker_hpp
#define GUARD_reconciliation_status_marker_hpp

#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

/**
 * @param p_is_reconciled the reconciliation status we want to represent
 * (\e true for reconciled or \e false for unreconciled).
 *
 * @returns a very brief string (possibly empty) suitable for use in a GUI
 * control, for indicating the reconciliation status passed to \e
 * p_is_reconciled.
 */
wxString reconciliation_status_marker(bool p_is_reconciled);


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_reconciliation_status_marker_hpp
