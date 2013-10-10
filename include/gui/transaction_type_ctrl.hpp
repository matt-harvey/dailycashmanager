/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_transaction_type_ctrl_hpp_8775560031074826
#define GUARD_transaction_type_ctrl_hpp_8775560031074826

#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

// End forward declarations

namespace gui
{

/**
 * Widget to enable the user to select a TransactionType.
 */
class TransactionTypeCtrl: public wxComboBox
{
public:

	/**
	 * p_parent must be a TransactionCtrl*. Make sure that the vector
	 * passed to p_transaction_types does not contain any TransactionTypes
	 * that are not in available_transaction_types(p_database_connection) -
	 * this is the caller's responsibility.
	 * p_transaction_types should not be empty.
	 */
	TransactionTypeCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		std::vector<TransactionType> const&
			p_transaction_types
	);

	TransactionTypeCtrl(TransactionTypeCtrl const&) = delete;
	TransactionTypeCtrl(TransactionTypeCtrl&&) = delete;
	TransactionTypeCtrl& operator=(TransactionTypeCtrl const&) = delete;
	TransactionTypeCtrl& operator=(TransactionTypeCtrl&&) = delete;
	virtual ~TransactionTypeCtrl();

	/**
	 * @returns the currently selected TransactionType, wrapped in a
	 * boost::optional. This may be an uninitialized optional in case
	 * we are in Windows and the underlying wxComboBox is in a
	 * transitional state.
	 */
	boost::optional<TransactionType>
	transaction_type() const;

	/**
	 * Sets the TransactionType displayed in the control.
	 */
	void set_transaction_type
	(	TransactionType p_transaction_type
	);

private:
	void on_kill_focus(wxFocusEvent& event);
	void on_change(wxCommandEvent& event);

	std::vector<TransactionType> m_transaction_types;
	PhatbooksDatabaseConnection& m_database_connection;

	DECLARE_EVENT_TABLE()

};  // class TransactionTypeCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_transaction_type_ctrl_hpp_8775560031074826
