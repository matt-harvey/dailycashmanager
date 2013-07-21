#ifndef GUARD_unfiltered_entry_list_ctrl_hpp
#define GUARD_unfiltered_entry_list_ctrl_hpp

#include "entry_list_ctrl.hpp"

namespace phatbooks
{
namespace gui
{

/**
 * An EntryListCtrl which is not filtered by Account (but which may be
 * filtered by date).
 */
class UnfilteredEntryListCtrl: public EntryListCtrl
{
public:
	virtual ~UnfilteredEntryListCtrl();

private:

};  // class UnfilteredEntryListCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_unfiltered_entry_list_ctrl_hpp
