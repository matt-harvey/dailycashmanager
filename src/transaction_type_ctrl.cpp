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

#include "gui/transaction_type_ctrl.hpp"
#include "string_conv.hpp"
#include "dcm_database_connection.hpp"
#include "transaction_type.hpp"
#include "gui/combo_box.hpp"
#include "gui/string_set_validator.hpp"
#include "gui/transaction_ctrl.hpp"
#include <boost/optional.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <wx/arrstr.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(TransactionTypeCtrl, ComboBox)
    EVT_COMBOBOX(wxID_ANY, TransactionTypeCtrl::on_change)
END_EVENT_TABLE()

TransactionTypeCtrl::TransactionTypeCtrl
(   wxWindow* p_parent,
    wxWindowID p_id,
    wxSize const& p_size,
    DcmDatabaseConnection& p_database_connection,
    vector<TransactionType> const& p_transaction_types
):
    ComboBox
    (   p_parent,
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
        TransactionType const ttype = transaction_type_from_verb(GetValue());
        assert_transaction_type_validity(ttype);
        ret = ttype;
    }
    return ret;
}

void
TransactionTypeCtrl::set_transaction_type(TransactionType p_transaction_type)
{
    for (int i = 0; ; ++i)
    {
        TransactionType const ttype = m_transaction_types[i];
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
TransactionTypeCtrl::on_change(wxCommandEvent& event)
{
    (void)event;  // silence compiler re. unused param.
    TransactionCtrl* parent = dynamic_cast<TransactionCtrl*>(GetParent());
    JEWEL_ASSERT (parent);
    optional<TransactionType> const maybe_ttype = transaction_type();
    if (!maybe_ttype)
    {
        return;
    }
    assert_transaction_type_validity(value(maybe_ttype));
    parent->refresh_for_transaction_type(value(maybe_ttype));
    return;
}

}  // namespace gui
}  // namespace dcm
