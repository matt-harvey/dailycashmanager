/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_transaction_type_ctrl_hpp_8775560031074826
#define GUARD_transaction_type_ctrl_hpp_8775560031074826

#include "transaction_type.hpp"
#include "gui/combo_box.hpp"
#include <boost/optional.hpp>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

namespace dcm
{

// Begin forward declarations

class DcmDatabaseConnection;

// End forward declarations

namespace gui
{

/**
 * Widget to enable the user to select a TransactionType.
 *
 * Note, there is no way to insert additional TransactionTypes
 * into a TransactionTypeCtrl after it has been constructed.
 */
class TransactionTypeCtrl: public ComboBox
{
public:

	/**
	 * \e p_parent must be a TransactionCtrl*. Make sure that the vector
	 * passed to p_transaction_types does not contain any TransactionTypes
	 * that are not in available_transaction_types(p_database_connection) -
	 * this is the caller's responsibility.
	 *
	 * \e p_transaction_types should not be empty.
	 */
	TransactionTypeCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size,
		DcmDatabaseConnection& p_database_connection,
		std::vector<TransactionType> const& p_transaction_types
	);

	TransactionTypeCtrl(TransactionTypeCtrl const&) = delete;
	TransactionTypeCtrl(TransactionTypeCtrl&&) = delete;
	TransactionTypeCtrl& operator=(TransactionTypeCtrl const&) = delete;
	TransactionTypeCtrl& operator=(TransactionTypeCtrl&&) = delete;
	virtual ~TransactionTypeCtrl();

	/**
	 * @returns the currently selected TransactionType, wrapped in a
	 * boost::optional. This may be an uninitialized optional in case
	 * we are in Windows and the underlying ComboBox is in a
	 * transitional state.
	 */
	boost::optional<TransactionType> transaction_type() const;

	/**
	 * Sets the TransactionType displayed in the control.
	 */
	void set_transaction_type(TransactionType p_transaction_type);

private:
	void on_change(wxCommandEvent& event);
	std::vector<TransactionType> m_transaction_types;
	DcmDatabaseConnection& m_database_connection;
	DECLARE_EVENT_TABLE()

};  // class TransactionTypeCtrl

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_transaction_type_ctrl_hpp_8775560031074826
