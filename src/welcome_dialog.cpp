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

#include "gui/welcome_dialog.hpp"
#include "app.hpp"
#include "dcm_database_connection.hpp"
#include "gui/button.hpp"
#include <jewel/assert.hpp>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <cmath>

using std::max;

namespace dcm
{
namespace gui
{

BEGIN_EVENT_TABLE(WelcomeDialog, wxDialog)
    EVT_BUTTON
    (   s_existing_file_button_id,
        WelcomeDialog::on_existing_file_button_click
    )
    EVT_BUTTON
    (   s_new_file_button_id,
        WelcomeDialog::on_new_file_button_click
    )
    EVT_BUTTON
    (   wxID_CANCEL,
        WelcomeDialog::on_cancel_button_click
    )
END_EVENT_TABLE()



WelcomeDialog::WelcomeDialog
(   DcmDatabaseConnection& p_database_connection
):
    wxDialog
    (   0,
        wxID_ANY,
        wxString("Welcome to ") + App::application_name() + wxString("!"),
        wxDefaultPosition,
        wxSize(wxDefaultSize.x * 2, wxDefaultSize.y * 2),
        wxRESIZE_BORDER
    ),
    m_database_connection(p_database_connection),
    m_user_wants_new_file(false),
    m_top_sizer(nullptr),
    m_existing_file_button(nullptr),
    m_new_file_button(nullptr),
    m_cancel_button(nullptr)    
{
    m_top_sizer = new wxGridSizer(7, 1, 0, 0);
    SetSizer(m_top_sizer);
    configure_buttons();
    m_top_sizer->Fit(this);
    m_top_sizer->SetSizeHints(this);
    m_existing_file_button->SetFocus();
    Layout();
    CentreOnScreen();    
}

WelcomeDialog::~WelcomeDialog()
{
}

void
WelcomeDialog::configure_buttons()
{
    wxString const file_descriptor =
        App::application_name() + wxString(" file");
    m_top_sizer->AddStretchSpacer();

    // TODO LOW PRIORITY The below code ensures the buttons are the same width,
    // but is dependent on the relative length of the strings in each button.
    // This sucks; but the better way, viz. to call SetSize(...) to resize
    // whichever is the smallest button after they have been created, doesn't
    // seem to work. SetSize(...) doesn't seem to do anything. Should find a
    // better way.
    m_existing_file_button = new Button
    (   this,
        s_existing_file_button_id,
        wxString("Open an &existing ") + file_descriptor
    );
    m_new_file_button = new Button
    (   this,
        s_new_file_button_id,
        wxString("Create a &new ") + file_descriptor,
        wxDefaultPosition,
        m_existing_file_button->GetSize()
    );
    m_cancel_button = new Button
    (   this,
        wxID_CANCEL,
        wxString("&Cancel"),
        wxDefaultPosition,
        m_existing_file_button->GetSize()
    );
    JEWEL_ASSERT
    (   m_new_file_button->GetSize() ==
        m_existing_file_button->GetSize()
    );
    JEWEL_ASSERT
    (   m_cancel_button->GetSize() ==
        m_existing_file_button->GetSize()
    );
    // Use sizer to position buttons on dialog
    m_top_sizer->Add
    (   m_existing_file_button,
        1,
        wxALIGN_CENTER | wxLEFT | wxRIGHT,
        20
    );
    m_top_sizer->AddStretchSpacer();
    m_top_sizer->Add
    (   m_new_file_button,
        1,
        wxALIGN_CENTER | wxLEFT | wxRIGHT,
        20
    );
    m_top_sizer->AddStretchSpacer();
    m_top_sizer->Add
    (   m_cancel_button,
        1,
        wxALIGN_CENTER | wxLEFT | wxRIGHT,
        20
    );
    m_top_sizer->AddStretchSpacer();
}


void
WelcomeDialog::on_existing_file_button_click(wxCommandEvent& event)
{
    JEWEL_ASSERT (!m_user_wants_new_file);
    EndModal(wxID_OK);
    (void)event;  // Silence compiler warning about unused parameter.
    return;
}


void
WelcomeDialog::on_new_file_button_click(wxCommandEvent& event)
{
    m_user_wants_new_file = true;
    EndModal(wxID_OK);
    (void)event;  // Silence compiler warning about unused parameter.
    return;
}


void
WelcomeDialog::on_cancel_button_click(wxCommandEvent& event)
{
    JEWEL_ASSERT (!m_user_wants_new_file);
    EndModal(wxID_CANCEL);
    (void)event;  // Silence compiler warning about unused parameter.
    return;
}


bool
WelcomeDialog::user_wants_new_file() const
{
    return m_user_wants_new_file;
}

}  // namespace gui
}  // namespace dcm

