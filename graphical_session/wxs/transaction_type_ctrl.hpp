// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_transaction_type_ctrl_hpp
#define GUARD_transaction_type_ctrl_hpp

#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>

namespace phatbooks
{
namespace gui
{

/**
 * Widget to enable the user to select a TransactionType.
 */
class TransactionTypeCtrl: public wxComboBox
{
public:

	/**
	 * p_parent must be a TransactionCtrl*.
	 */
	TransactionTypeCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size
	);

	/**
	 * @returns the currently selected TransactionType, wrapped in a
	 * boost::optional. This may be an uninitialized optional in case
	 * we are in Windows and the underlying wxComboBox is in a
	 * transitional state.
	 */
	boost::optional<transaction_type::TransactionType>
	transaction_type() const;

	/**
	 * Sets the TransactionType displayed in the control.
	 */
	void set_transaction_type
	(	transaction_type::TransactionType p_transaction_type
	);

private:
	void on_kill_focus(wxFocusEvent& event);
	void on_change(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

};  // class TransactionTypeCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_type_ctrl_hpp