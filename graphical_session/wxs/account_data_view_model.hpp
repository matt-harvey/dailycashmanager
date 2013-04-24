#ifndef GUARD_account_data_view_model_hpp
#define GUARD_account_data_view_model_hpp

#include <wx/dataview.h>

namespace phatbooks
{
namespace gui
{

/**
 * This class, by subclassing wxDataViewModel, facilites
 * the presentation of phatbooks::Account-related information
 * in the form of a wxDataViewCtrl.
 */
class AccountDataViewModel:
	public wxDataViewModel
{
public:

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 * Indicates whether \e item is a container, i.e. can have
	 * child items.
	 */
	virtual bool IsContainer(wxDataViewItem const& item) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 *
	 * @returns the wxDataViewItem represents the parent of item, or
	 * an invalid wxDataViewItem if the root item is the parent
	 * item.
	 */
	virtual wxDataViewItem GetParent(wxDataViewItem const& item) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 */
	virtual unsigned int GetChildren
	(	wxDataViewItem const&,
		wxDataViewItemArray& children
	) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 *
	 * Indicates the number of columns in the model.
	 */
	virtual unsigned int GetColumnCount() const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 *
	 * Indicates what type of data is stored in the column
	 * specified by \e col.
	 */
	virtual wxString GetColumnType(unsigned int col) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 */
	virtual void GetValue
	(	wxVariant& variant,
		wxDataViewItem const& item,
		unsigned int col
	) const;
	
private:

};  // AccountDataViewModel


}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_account_data_view_model_hpp
