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


#include "gui/transaction_type_ctrl.hpp"
#include "string_conv.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include "gui/string_set_validator.hpp"
#include "gui/transaction_ctrl.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;


// For debugging
	#include <jewel/log.hpp>
	#include <iostream>
	using std::endl;


namespace phatbooks
{

namespace gui
{

BEGIN_EVENT_TABLE(TransactionTypeCtrl, wxComboBox)
	EVT_KILL_FOCUS
	(	TransactionTypeCtrl::on_kill_focus
	)
	EVT_COMBOBOX
	(	wxID_ANY,
		TransactionTypeCtrl::on_change
	)
END_EVENT_TABLE()


TransactionTypeCtrl::TransactionTypeCtrl
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	vector<TransactionType> const& p_transaction_types
):
	wxComboBox
	(	p_parent,
		p_id,
		transaction_type_to_verb(static_cast<TransactionType>(0)),
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY
	),
	m_transaction_types(p_transaction_types),
	m_database_connection(p_database_connection)
{
	JEWEL_ASSERT (!m_transaction_types.empty());
	// TODO We need a mechanism whereby, if additional TransactionTypes
	// become available after the TransactionTypeCtrl has been
	// constructed, the TransactionTypeCtrl is notified and updates
	// itself accordingly.
	for (TransactionType const& elem: m_transaction_types)
	{
		wxString const verb = transaction_type_to_verb(elem);
		Append(verb);  // add to combobox
	}
	SetSelection(0);  // In effort to avoid apparent bug in Windows.
}

TransactionTypeCtrl::~TransactionTypeCtrl()
{
}

optional<TransactionType>
TransactionTypeCtrl::transaction_type() const
{
	optional<TransactionType> ret;
	if (GetSelection() >= 0)
	{
		TransactionType const ttype =
			transaction_type_from_verb(GetValue());
		assert_transaction_type_validity(ttype);
		ret = ttype;
	}
	return ret;
}

void
TransactionTypeCtrl::set_transaction_type
(	TransactionType p_transaction_type
)
{
	for (int i = 0; ; ++i)
	{
		TransactionType const ttype =
			m_transaction_types[i];
		if (ttype == p_transaction_type)
		{
			SetSelection(i);
			return;
		}
		if (i == static_cast<int>(TransactionType::num_transaction_types))
		{
			JEWEL_HARD_ASSERT (false);
			return;
		}
	}
}

void
TransactionTypeCtrl::on_kill_focus(wxFocusEvent& event)
{
	// TODO Make a class from which we can privately inherit, to capture
	// this on_kill_focus behaviour, which is shared by several custom
	// widget classes in phatbooks::gui.

	// Unfortunately if we call Validate() and TransferDataToWindow()
	// directly on the AccountCtrl, it doesn't work. We have to call
	// through parent instead.
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();
	event.Skip();
	return;
}

void
TransactionTypeCtrl::on_change(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused param.
	TransactionCtrl* parent = dynamic_cast<TransactionCtrl*>(GetParent());
	JEWEL_ASSERT (parent);
	optional<TransactionType> const maybe_ttype =
		transaction_type();
	if (!maybe_ttype)
	{
		return;
	}
	assert_transaction_type_validity(value(maybe_ttype));
	parent->refresh_for_transaction_type(value(maybe_ttype));
	return;
}


}  // namespace gui
}  // namespace phatbooks
