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

#include "gui/setup_wizard.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "app.hpp"
#include "augmented_account.hpp"
#include "commodity.hpp"
#include "filename_validation.hpp"
#include "finformat.hpp"
#include "make_currencies.hpp"
#include "make_default_accounts.hpp"
#include "ordinary_journal.hpp"
#include "dcm_database_connection.hpp"
#include "dcm_exceptions.hpp"
#include "string_conv.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include "gui/button.hpp"
#include "gui/client_data.hpp"
#include "gui/date_ctrl.hpp"
#include "gui/combo_box.hpp"
#include "gui/frame.hpp"
#include "gui/locale.hpp"
#include "gui/multi_account_panel.hpp"
#include "gui/sizing.hpp"
#include "gui/text_ctrl.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/handle.hpp>
#include <wx/arrstr.h>
#include <wx/bitmap.h>
#include <wx/dataview.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/gbsizer.h>
#include <wx/msgdlg.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/treelist.h>
#include <wx/validate.h>
#include <wx/variant.h>
#include <wx/window.h>
#include <wx/wizard.h>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <set>
#include <string>
#include <vector>

#include "../images/icon_48_48.xpm"

using boost::optional;
using jewel::Decimal;
using jewel::Log;
using jewel::value;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;
using std::copy;
using std::back_inserter;
using std::ostringstream;
using std::set;
using std::string;
using std::vector;

// For debugging
#include <jewel/log.hpp>
#include <iostream>
using std::endl;

namespace filesystem = boost::filesystem;
namespace gregorian = boost::gregorian;

namespace dcm
{
namespace gui
{

namespace
{
    filesystem::path wx_to_boost_filepath
    (   wxString const& wx_directory,
        wxString const& wx_filename
    )
    {
        JEWEL_LOG_TRACE();
        filesystem::path const directory =
            filesystem::path(wx_to_std8(wx_directory));
        filesystem::path const filename =
            filesystem::path(wx_to_std8(wx_filename));
        filesystem::path ret = directory;
        ret /= filename;
        JEWEL_LOG_TRACE();
        return ret;
    }

    wxString with_extension(wxString const& s)
    {
        filesystem::path const path(wx_to_std8(s));
        if (std8_to_wx(path.extension().string()) == App::filename_extension())
        {
            return s;
        }
        return s + App::filename_extension();
    }

    wxString without_extension(wxString const& s)
    {
        filesystem::path const path(wx_to_std8(s));
        if (std8_to_wx(path.extension().string()) == App::filename_extension())
        {
            if (path.extension() != path)
            {
                return std8_to_wx(path.stem().string());
            }
        }
        return s;
    }

}  // end anonymous namespace


/*** SetupWizard ***/


SetupWizard::SetupWizard
(   DcmDatabaseConnection& p_database_connection
):
    wxWizard
    (   0,
        wxID_ANY,
        App::application_name() + wxString(" Setup Wizard"),
        wxBitmap(icon_48_48_xpm),
        wxDefaultPosition,
        wxDEFAULT_DIALOG_STYLE | wxFULL_REPAINT_ON_RESIZE
    ),
    m_database_connection(p_database_connection),
    m_filepath_page(nullptr),
    m_balance_sheet_account_page(nullptr),
    m_pl_account_page(nullptr)
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (m_account_names_already_taken.empty());
    JEWEL_ASSERT (!m_database_connection.is_valid());
    m_filepath_page = new FilepathPage(this, m_database_connection);
    m_balance_sheet_account_page = new AccountPage
    (   this,
        AccountSuperType::balance_sheet,
        m_database_connection
    );
    m_pl_account_page = new AccountPage
    (   this,
        AccountSuperType::pl,
        m_database_connection
    );
    render_account_pages();
    wxWizardPageSimple::Chain
    (   m_filepath_page,
        m_balance_sheet_account_page
    );
    wxWizardPageSimple::Chain
    (   m_balance_sheet_account_page,
        m_pl_account_page
    );
    GetPageAreaSizer()->Add(m_filepath_page);
    JEWEL_LOG_TRACE();
}

SetupWizard::~SetupWizard()
{
    JEWEL_LOG_TRACE();
}

void
SetupWizard::run()
{
    JEWEL_LOG_TRACE();
    if (RunWizard(m_filepath_page))
    {
        JEWEL_LOG_TRACE();
        // Then user completed Wizard rather than cancelling.
        configure_default_commodity();
        configure_entity_creation_date();
        create_file();
        try
        {
            JEWEL_LOG_TRACE();
            configure_accounts();
        }
        catch (...)
        {
            JEWEL_LOG_TRACE();
            delete_file();
            JEWEL_LOG_TRACE();
            throw;
        }
        JEWEL_LOG_TRACE();
    }
    JEWEL_LOG_TRACE();
    return;
}

wxSize
SetupWizard::standard_text_box_size()
{
    return wxSize(140, 12);
}

Handle<Commodity>
SetupWizard::selected_currency() const
{
    return m_filepath_page->selected_currency();
}

gregorian::date
SetupWizard::selected_start_date() const
{
    return m_filepath_page->selected_start_date();
}

Decimal
SetupWizard::total_opening_balance() const
{
    JEWEL_ASSERT (m_balance_sheet_account_page);
    return m_balance_sheet_account_page->total_amount();
}

void
SetupWizard::set_account_names_already_taken(set<wxString> const& p_names)
{
    m_account_names_already_taken.clear();
    for (auto const& name: p_names)
    {
        JEWEL_LOG_MESSAGE(Log::info, "Recording Account name to SetupWizard.");
        JEWEL_LOG_VALUE(Log::info, name);
        m_account_names_already_taken.insert(name);
    }
    return;
}

set<wxString> const&
SetupWizard::account_names_already_taken() const
{
    return m_account_names_already_taken;
}

void
SetupWizard::set_assumed_currency(Handle<Commodity> const& p_commodity)
{
    JEWEL_ASSERT (m_balance_sheet_account_page);    
    JEWEL_LOG_VALUE(Log::info, static_cast<size_t>(p_commodity->precision()));
    m_balance_sheet_account_page->set_commodity(p_commodity);
    m_pl_account_page->set_commodity(p_commodity);
    return;
}

vector<AugmentedAccount>
SetupWizard::augmented_accounts() const
{
    vector<AugmentedAccount> ret =
        m_balance_sheet_account_page->augmented_accounts();
    auto const more = m_pl_account_page->augmented_accounts();
    copy(more.begin(), more.end(), back_inserter(ret));
    return ret;
}

void
SetupWizard::render_account_pages()
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (m_balance_sheet_account_page);
    JEWEL_ASSERT (m_pl_account_page);
    m_balance_sheet_account_page->render();
    m_pl_account_page->render();
    return;
}

void
SetupWizard::configure_default_commodity()
{
    JEWEL_LOG_TRACE();
    Handle<Commodity> const commodity = selected_currency();
    commodity->set_multiplier_to_base(Decimal(1, 0));
    m_database_connection.set_default_commodity(commodity);
    return;
}

void
SetupWizard::configure_entity_creation_date()
{
    JEWEL_LOG_TRACE();
    m_database_connection.
        set_entity_creation_date(selected_start_date());
    return;
}

void
SetupWizard::create_file()
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (m_filepath_page);
    JEWEL_ASSERT (m_filepath_page->selected_filepath());
    m_database_connection.open(value(m_filepath_page->selected_filepath()));
    return;
}

void
SetupWizard::delete_file()
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (m_filepath_page);
    boost::filesystem::remove(value(m_filepath_page->selected_filepath()));
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::configure_accounts()
{
    DatabaseTransaction transaction(m_database_connection);
    for (AugmentedAccount& aug_acc: augmented_accounts())
    {
        wxString const name_wx = aug_acc.account->name().Trim();
        if (!name_wx.IsEmpty())
        {
            aug_acc.account->set_commodity(selected_currency());
            aug_acc.account->set_visibility(Visibility::visible);
            aug_acc.account->save();
            JEWEL_ASSERT 
            (   aug_acc.technical_opening_balance.places() ==
                selected_currency()->precision()
            );
            Handle<OrdinaryJournal> const opening_balance_journal =
            create_opening_balance_journal
            (   aug_acc.account,
                aug_acc.technical_opening_balance
            );
            opening_balance_journal->save();
        }
    }
    transaction.commit();
    return;
}


/*** SetupWizard::FilepathValidator ***/

SetupWizard::FilepathValidator::FilepathValidator
(   filesystem::path* p_filepath
):
    m_filepath(p_filepath)
{
}

SetupWizard::FilepathValidator::FilepathValidator
(   FilepathValidator const& rhs
):
    wxValidator(),
    m_filepath(rhs.m_filepath)
{
}

SetupWizard::FilepathValidator::~FilepathValidator()
{
}

bool
SetupWizard::FilepathValidator::Validate(wxWindow* WXUNUSED(parent))
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    TextCtrl const* const text_ctrl =
        dynamic_cast<TextCtrl*>(GetWindow());    
    if (!text_ctrl)
    {
        JEWEL_LOG_TRACE();
        return false;
    }
    wxString const wx_filename = with_extension(text_ctrl->GetValue());
    FilepathPage* page =
        dynamic_cast<FilepathPage*>(text_ctrl->GetParent());
    JEWEL_ASSERT (page);
    if (!page)
    {
        JEWEL_LOG_TRACE();
        return false;
    }
    JEWEL_ASSERT (page->m_directory_ctrl);
    wxString const wx_directory    = page->m_directory_ctrl->GetValue();
    filesystem::path const path =
        wx_to_boost_filepath(wx_directory, wx_filename);
    JEWEL_LOG_VALUE(Log::info, wx_directory);
    JEWEL_LOG_VALUE(Log::info, wx_filename);
    string filename_error_message;
    bool const filename_is_valid = is_valid_filename
    (   wx_to_std8(wx_filename),
        filename_error_message,
        false  // We don't want explicit extension
    );
    bool const directory_exists =
        filesystem::exists(filesystem::status(path.parent_path()));
    bool const filepath_already_exists = filesystem::exists(path);
    bool const ret =
    (   filename_is_valid &&
        directory_exists &&
        !filepath_already_exists
    );
    if (!ret)
    {
        if (!filename_is_valid)
        {
            wxString const message = std8_to_wx(filename_error_message);
            JEWEL_ASSERT (!message.IsEmpty());
            wxMessageBox(message);
        }
        if (!directory_exists)
        {
            wxMessageBox("Folder does not exist.");
        }
        if (filepath_already_exists)
        {
            wxMessageBox
            (   wxString("File named ") +
                wx_filename +
                wxString(" already exists in ") +
                wx_directory +
                wxString(".")
            );
        }
        JEWEL_LOG_TRACE();
        return false;
    }
    if (m_filepath)
    {
        JEWEL_LOG_TRACE();
        *m_filepath = path;
    }
    JEWEL_LOG_TRACE();
    return ret;
}

bool
SetupWizard::FilepathValidator::TransferFromWindow()
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    if (m_filepath)
    {
        FilepathPage const* const page =
            dynamic_cast<FilepathPage*>(GetWindow()->GetParent());
        if (!page)
        {
            JEWEL_LOG_TRACE();
            return false;
        }
        optional<filesystem::path> const path =
            page->selected_filepath();
        if (path) *m_filepath = value(path);
    }
    JEWEL_LOG_TRACE();
    return true;
}

bool
SetupWizard::FilepathValidator::TransferToWindow()
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (GetWindow()->IsKindOf(CLASSINFO(TextCtrl)));
    if (m_filepath)
    {
        TextCtrl* const text_ctrl =
            dynamic_cast<TextCtrl*>(GetWindow());
        if (!text_ctrl)
        {
            JEWEL_LOG_TRACE();
            return false;
        }
        text_ctrl->SetValue
        (   without_extension
            (   std8_to_wx(m_filepath->filename().string())
            )
        );
    }
    JEWEL_LOG_TRACE();
    return true;
}

wxObject*
SetupWizard::FilepathValidator::Clone() const
{
    return new FilepathValidator(*this);
}


/*** SetupWizard::FilepathPage ***/

BEGIN_EVENT_TABLE(SetupWizard::FilepathPage, wxWizardPageSimple)
    EVT_BUTTON
    (   s_directory_button_id,
        SetupWizard::FilepathPage::on_directory_button_click
    )
    EVT_WIZARD_PAGE_CHANGING
    (   wxID_ANY,
        SetupWizard::FilepathPage::on_wizard_page_changing
    )
    EVT_WIZARD_PAGE_CHANGED
    (   wxID_ANY,
        SetupWizard::FilepathPage::on_wizard_page_changed
    )
    EVT_COMBOBOX
    (   s_currency_box_id,
        SetupWizard::FilepathPage::on_currency_selection_change
    )
    EVT_COMBOBOX
    (   s_precision_box_id,
        SetupWizard::FilepathPage::on_precision_selection_change
    )
END_EVENT_TABLE()

SetupWizard::FilepathPage::FilepathPage
(   SetupWizard* parent,
    DcmDatabaseConnection& p_database_connection
):
    wxWizardPageSimple(parent),
    m_currencies(make_currencies(p_database_connection)),
    m_top_sizer(nullptr),
    m_filename_row_sizer(nullptr),
    m_directory_row_sizer(nullptr),
    m_directory_ctrl(nullptr),
    m_directory_button(nullptr),
    m_filename_ctrl(nullptr),
    m_currency_box(nullptr),
    m_precision_box(nullptr),
    m_selected_filepath(nullptr)
{
    JEWEL_LOG_TRACE();
    m_top_sizer = new wxBoxSizer(wxVERTICAL);
    m_filename_row_sizer = new wxBoxSizer(wxHORIZONTAL);
    m_directory_row_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxSize const dlg_unit_size = SetupWizard::standard_text_box_size();

    // First row
    wxStaticText* directory_prompt = new wxStaticText
    (   this,
        wxID_ANY,
        wxString("Folder where new file should be saved:"),
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_LEFT
    );
    m_top_sizer->Add(directory_prompt);

    // Second row
    m_top_sizer->Add(m_directory_row_sizer);
    wxString default_directory = wxEmptyString;
    optional<filesystem::path> const maybe_directory = App::default_directory();
    if (maybe_directory)
    {
        default_directory = std8_to_wx(value(maybe_directory).string());
        JEWEL_ASSERT (!m_selected_filepath);
        m_selected_filepath = new filesystem::path(value(maybe_directory));
    }
    // TODO MEDIUM PRIORITY We should make this a static text field or
    // something that is obviously noneditable so the user doesn't feel
    // frustrated when they try to edit it.
    m_directory_ctrl = new TextCtrl
    (   this,
        wxID_ANY,
        default_directory,
        wxDefaultPosition,
        wxDLG_UNIT(this, dlg_unit_size),
        wxTE_READONLY,  // style
        wxDefaultValidator  // not user-editable anyway...
    );
    m_directory_button = new Button
    (   this,
        s_directory_button_id,
        wxString("&Browse..."),
        wxDefaultPosition,
        wxSize(wxDefaultSize.x, m_directory_ctrl->GetSize().y)
    );
    m_directory_row_sizer->Add(m_directory_ctrl, wxSizerFlags(1).Expand());
    m_directory_row_sizer->Add(m_directory_button, 0, wxLEFT, 5);

    // Third row
    m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);

    // Fourth row
    wxStaticText* filename_prompt = new wxStaticText
    (   this,
        wxID_ANY,
        wxString("Name for new file:"),
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_LEFT
    );
    m_top_sizer->Add(filename_prompt);

    // Fifth row
    m_top_sizer->Add(m_filename_row_sizer);
    wxString const ext = App::filename_extension();
    m_filename_ctrl = new TextCtrl
    (   this,
        wxID_ANY,
        wxString("MyBudget"),
        wxDefaultPosition,
        wxDLG_UNIT(this, dlg_unit_size),
        0,  // style
        FilepathValidator(m_selected_filepath)
    );
    m_filename_row_sizer->Add(m_filename_ctrl, wxSizerFlags(1).Expand());
    wxStaticText* extension_text = new wxStaticText
    (   this,
        wxID_ANY,
        ext,
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_LEFT | wxALIGN_BOTTOM
    );
    m_filename_row_sizer->Add(extension_text);

    // Sixth row
    m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);

    // Seventh row
    wxStaticText* currency_prompt = new wxStaticText
    (   this,
        wxID_ANY,
        wxString("Select your currency"),
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_LEFT
    );
    m_top_sizer->Add(currency_prompt);

    // Eighth row
    m_currency_box = new ComboBox
    (   this,
        s_currency_box_id,
        wxEmptyString,
        wxDefaultPosition,
        wxDLG_UNIT(this, dlg_unit_size),
        0,
        0,
        wxCB_DROPDOWN | wxCB_SORT | wxCB_READONLY
    );
    size_t max_precision = 0;
    for (Handle<Commodity> const& commodity: m_currencies)
    {
        unsigned int const item_index = m_currency_box->Append
        (   commodity->name() +
            wxString(" (") +
            commodity->abbreviation() +
            wxString(")")
        );
        if (commodity->precision() > max_precision)
        {
            max_precision = commodity->precision();
        }
        auto const data = new ClientData<Handle<Commodity> >(commodity);

        // Note control will take care of memory management
        // Note also that the association of each item with a particular
        // Commodity stays constant even while the order of items is
        // changed due to sorting - which is what we want.
        m_currency_box->SetClientObject(item_index, data);
    }
    m_currency_box->SetSelection(0);
    m_top_sizer->Add(m_currency_box);

    // Ninth row
    m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);

    // Tenth row
    wxStaticText* precision_prompt = new wxStaticText
    (   this,
        wxID_ANY,
        wxString("Confirm the precision of your currency"),
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_LEFT
    );
    m_top_sizer->Add(precision_prompt);

    // Eleventh row
    wxArrayString precision_descriptions;
    for (unsigned char i = 0; i != max_precision + 1; ++i)
    {
        precision_descriptions.Add
        (   finformat_wx
            (   Decimal(0, i),
                locale(),
                DecimalFormatFlags().clear(string_flags::dash_for_zero)
            )
        );
    }
    JEWEL_ASSERT (!precision_descriptions.IsEmpty());
    m_precision_box = new ComboBox
    (   this,
        s_precision_box_id,
        wxEmptyString,
        wxDefaultPosition,
        wxDLG_UNIT(this, dlg_unit_size),
        0,
        0,
        wxCB_DROPDOWN | wxCB_SORT | wxCB_READONLY
    );
    m_precision_box->Set(precision_descriptions);
    m_precision_box->SetSelection(selected_currency()->precision());
    m_top_sizer->Add(m_precision_box);

    // Twelfth row
    m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);

    // Thirteenth row
    wxStaticText* start_date_prompt = new wxStaticText
    (   this,
        wxID_ANY,
        wxString
        (   "Enter a starting date. (You will enter transactions\n"
            "from this date onwards.)"
        ),
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_LEFT
    );
    m_top_sizer->Add(start_date_prompt);

    // Fourteenth row
    m_start_date_ctrl = new DateCtrl
    (   this,
        s_start_date_ctrl_id,
        wxDLG_UNIT(this, dlg_unit_size)
    );
    m_top_sizer->Add(m_start_date_ctrl);

    // Additional space at bottom
    for (size_t i = 0; i != 2; ++i)
    {
        m_top_sizer->AddSpacer(m_directory_ctrl->GetSize().y);
    }

    SetSizer(m_top_sizer);
    m_top_sizer->Fit(this);
    Layout();
    JEWEL_LOG_TRACE();
}

SetupWizard::FilepathPage::~FilepathPage()
{
    JEWEL_LOG_TRACE();
    delete m_selected_filepath;
    m_selected_filepath = nullptr;
}

optional<filesystem::path>
SetupWizard::FilepathPage::selected_filepath() const
{
    JEWEL_LOG_TRACE();
    optional<filesystem::path> ret;
    if (m_selected_filepath)
    {
        JEWEL_LOG_TRACE();
        ret = *m_selected_filepath;
    }
    return ret;
}

Handle<Commodity>
SetupWizard::FilepathPage::selected_currency() const
{
    unsigned int const index = m_currency_box->GetSelection();
    auto const data = dynamic_cast<ClientData<Handle<Commodity> >* >
    (   m_currency_box->GetClientObject(index)
    );
    JEWEL_ASSERT (data != 0);
    return data->data();
}

gregorian::date
SetupWizard::FilepathPage::selected_start_date() const
{
    optional<gregorian::date> const maybe_start_date =
        m_start_date_ctrl->date();
    JEWEL_ASSERT (maybe_start_date);
    return value(maybe_start_date);
}

void
SetupWizard::FilepathPage::on_directory_button_click(wxCommandEvent& event)
{
    JEWEL_LOG_TRACE();
    wxString default_directory = m_directory_ctrl->GetValue();
    filesystem::path const default_path(wx_to_std8(default_directory));
    if (!filesystem::exists(filesystem::status(default_path)))
    {
        default_directory = wxEmptyString;
    }
    else
    {
        JEWEL_ASSERT (filesystem::absolute(default_path) == default_path);
    }
    wxDirDialog directory_dialog
    (   this,
        wxEmptyString,
        default_directory,
        wxDD_NEW_DIR_BUTTON
    );
    if (directory_dialog.ShowModal() == wxID_OK)
    {
        wxString const wx_directory = directory_dialog.GetPath();
        m_directory_ctrl->ChangeValue(wx_directory);
        JEWEL_LOG_TRACE();
        filesystem::path const fp = wx_to_boost_filepath
        (   wx_directory,
            with_extension(m_filename_ctrl->GetValue())
        );
        if (m_selected_filepath)
        {
            delete m_selected_filepath;
            m_selected_filepath = nullptr;
        }
        m_selected_filepath = new filesystem::path(fp);
        JEWEL_ASSERT (m_selected_filepath);
    }
    (void)event;  // Silence compiler warning about unused parameter.
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::FilepathPage::on_wizard_page_changing(wxWizardEvent& event)
{
    JEWEL_LOG_TRACE();
    (void)event;  // Silence compiler warning about unused parameter
    SetupWizard* const parent = dynamic_cast<SetupWizard*>(GetParent());
    JEWEL_ASSERT (parent);
    parent->set_assumed_currency(selected_currency());
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::FilepathPage::on_wizard_page_changed(wxWizardEvent& event)
{
    JEWEL_LOG_TRACE();
    (void)event;  // Silence compiler warning about unused parameter
    SetupWizard* const parent = dynamic_cast<SetupWizard*>(GetParent());
    JEWEL_ASSERT (parent);

    // Important to do this in case user is entering the FilepathPage by
    // turning backwards from AccountPages. We want to clear the
    // Account names already taken so that false Account name duplication error
    // messages aren't generated when the user turns back into the AccountPages.
    set<wxString> account_names;  // empty
    parent->set_account_names_already_taken(account_names);
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::FilepathPage::on_currency_selection_change(wxCommandEvent& event)
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (m_precision_box);
    m_precision_box->SetSelection(selected_currency()->precision());
    event.Skip();
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::FilepathPage::on_precision_selection_change(wxCommandEvent& event)
{
    JEWEL_LOG_TRACE();
    JEWEL_ASSERT (m_currency_box);
    JEWEL_ASSERT (m_precision_box);
    auto const sc = selected_currency();
    sc->set_precision(m_precision_box->GetSelection());
    event.Skip();
    JEWEL_LOG_TRACE();
    return;
}

/*** SetupWizard::AccountPage ***/

BEGIN_EVENT_TABLE(SetupWizard::AccountPage, wxWizardPageSimple)
    EVT_BUTTON
    (   s_remove_checked_button_id,
        SetupWizard::AccountPage::on_remove_checked_button_click
    )
    EVT_BUTTON
    (   s_push_row_button_id,
        SetupWizard::AccountPage::on_push_row_button_click
    )
    EVT_WIZARD_PAGE_CHANGING
    (   wxID_ANY,
        SetupWizard::AccountPage::on_wizard_page_changing
    )
    EVT_WIZARD_PAGE_CHANGED
    (   wxID_ANY,
        SetupWizard::AccountPage::on_wizard_page_changed
    )
    EVT_CHECKBOX
    (   wxID_ANY,
        SetupWizard::AccountPage::on_check_box_clicked
    )
END_EVENT_TABLE()

SetupWizard::AccountPage::AccountPage
(   SetupWizard* p_parent,
    AccountSuperType p_account_super_type,
    DcmDatabaseConnection& p_database_connection
):
    wxWizardPageSimple(p_parent),
    m_database_connection(p_database_connection),
    m_account_super_type(p_account_super_type),
    m_min_num_accounts(1),
    m_current_row(0),
    m_remove_checked_button(nullptr),
    m_push_row_button(nullptr),
    m_top_sizer(nullptr),
    m_multi_account_panel(nullptr),
    m_parent(*p_parent)
{
    JEWEL_LOG_TRACE();
}

SetupWizard::AccountPage::~AccountPage()
{
    JEWEL_LOG_TRACE();
}

void
SetupWizard::AccountPage::render()
{
    JEWEL_LOG_TRACE();
    m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
    SetSizer(m_top_sizer);
    increment_row();
    render_main_text();
    render_buttons();
    increment_row();
    increment_row();
    render_account_view();
    increment_row();
    m_top_sizer->Fit(this);
    Fit();
    Layout();
    JEWEL_LOG_TRACE();
}

vector<AugmentedAccount>
SetupWizard::AccountPage::augmented_accounts() const
{
    return m_multi_account_panel->augmented_accounts();
}

void
SetupWizard::AccountPage::set_commodity
(   Handle<Commodity> const& p_commodity
)
{
    JEWEL_ASSERT (m_multi_account_panel);
    m_multi_account_panel->set_commodity(p_commodity);
    return;
}

Decimal
SetupWizard::AccountPage::total_amount() const
{
    if (m_multi_account_panel)
    {
        return m_multi_account_panel->total_amount();
    }
    JEWEL_ASSERT (!m_multi_account_panel);
    return Decimal(0, 0);
}

Decimal
SetupWizard::AccountPage::total_balance_sheet_amount() const
{
    return parent().total_opening_balance();
}

void
SetupWizard::AccountPage::set_account_names_already_taken
(   set<wxString> const& p_account_names_already_taken
)
{
    JEWEL_ASSERT (m_multi_account_panel);
    m_multi_account_panel->
        set_account_names_already_taken(p_account_names_already_taken);
    return;
}

void
SetupWizard::AccountPage::render_main_text()
{
    JEWEL_LOG_TRACE();
    int const width = medium_width() * 3 + standard_gap();
    wxStaticText* text = new wxStaticText
    (   this,
        wxID_ANY,
        main_text(),
        wxDefaultPosition,
        wxSize(width, wxDefaultSize.y),
        wxALIGN_LEFT
    );
    text->Wrap(width);
    top_sizer().Add(text, wxGBPosition(current_row(), 0), wxGBSpan(2, 3));
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::AccountPage::render_buttons()
{
    JEWEL_LOG_TRACE();
    wxString const concept_name = account_concept_name(m_account_super_type);
    m_remove_checked_button = new Button
    (   this,
        s_remove_checked_button_id,
        wxString("Remove checked"),
        wxDefaultPosition,
        wxSize(medium_width(), wxDefaultSize.y),
        wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
    );
    top_sizer().Add
    (   m_remove_checked_button,
        wxGBPosition(current_row(), 3),
        wxDefaultSpan,
        wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
    );
    m_push_row_button = new Button
    (   this,
        s_push_row_button_id,
        wxString("Add ") + concept_name,
        wxDefaultPosition,
        wxSize(medium_width(), wxDefaultSize.y),
        wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
    );
    top_sizer().Add
    (   m_push_row_button,
        wxGBPosition(current_row(), 4),
        wxDefaultSpan,
        wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
    );
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::AccountPage::render_account_view()
{
    JEWEL_LOG_TRACE();

    // Create the control for displaying Accounts
    wxSize const size =
        wxDLG_UNIT(this, SetupWizard::standard_text_box_size());
    Handle<Commodity> const commodity = parent().selected_currency();

    // Add dummy column to right to allow room for scrollbar.
    wxStaticText* dummy = new wxStaticText
    (   this,
        wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxSize(scrollbar_width_allowance(), 1)
    );
    top_sizer().Add(dummy, wxGBPosition(current_row(), 5));

    // Main body of page.
    m_multi_account_panel = new MultiAccountPanel
    (   this,
        wxSize(MultiAccountPanel::required_width(), size.y * 14),
        database_connection(),
        m_account_super_type,
        commodity,
        m_min_num_accounts
    );
    top_sizer().Add
    (   m_multi_account_panel,
        wxGBPosition(current_row(), 0),
        wxGBSpan(1, 5)
    );

    refresh_remove_checked_button_state();

    // Add an empty row at bottom. This is a hack to prevent the scrolled
    // area from dropping off the bottom. Unclear on why this was happening -
    // but this fixes it.
    increment_row();
    wxStaticText* dummy2 = new wxStaticText
    (   this,
        wxID_ANY,
        wxEmptyString,
        wxDefaultPosition,
        wxSize(1, size.y)
    );
    top_sizer().Add(dummy2, wxGBPosition(current_row(), 0));

    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::AccountPage::refresh_remove_checked_button_state()
{
    JEWEL_ASSERT (m_multi_account_panel);
    JEWEL_ASSERT (m_multi_account_panel->num_rows() >= m_min_num_accounts);
    JEWEL_ASSERT (m_remove_checked_button);
    auto const num_checked = m_multi_account_panel->num_checked_rows();
    auto const num_rows = m_multi_account_panel->num_rows();
    if (num_rows - num_checked >= m_min_num_accounts)
    {
        m_remove_checked_button->Enable();
        m_remove_checked_button->SetToolTip(0);
    }
    else
    {
        JEWEL_ASSERT
        (   m_multi_account_panel->num_rows() ==
            m_min_num_accounts
        );
        m_remove_checked_button->Disable();
        ostringstream oss;
        oss << m_min_num_accounts;
        wxString const min_rows_string = std8_to_wx(oss.str());
        if (m_min_num_accounts != 1)
        {
            account_concept_name(m_account_super_type) += wxString("s");
        }
        m_remove_checked_button->SetToolTip
        (   wxString("You will need at least ") +
            min_rows_string +
            wxString(" ") +
            account_concept_name(m_account_super_type) +
            wxString(" to start off with.")
        );
    }
    return;
}

DcmDatabaseConnection&
SetupWizard::AccountPage::database_connection() const
{
    return m_database_connection;
}

SetupWizard const&
SetupWizard::AccountPage::parent() const
{
    return m_parent;
}

wxGridBagSizer&
SetupWizard::AccountPage::top_sizer()
{
    JEWEL_ASSERT (m_top_sizer);
    return *m_top_sizer;
}

int
SetupWizard::AccountPage::current_row() const
{
    return m_current_row;
}

void
SetupWizard::AccountPage::increment_row()
{
    ++m_current_row;
    return;
}

bool
SetupWizard::AccountPage::account_names_valid
(   wxString& error_message
) const
{
    JEWEL_ASSERT (m_multi_account_panel);
    return m_multi_account_panel->account_names_valid(error_message);
}

bool
SetupWizard::AccountPage::account_types_valid
(   wxString& error_message
) const
{
    // Forcing the user to create at least one balance sheet
    // Account, at least revenue Account and at least one
    // expense Account, means that all TransactionTypes can safely
    // be made available from the outset. This means we don't have
    // to bother with complex code to update TransactionTypeCtrl or
    // etc. depending on which TransactionTypes are available at any
    // given moment (assuming we also don't let the user delete
    // Accounts or change the TransactionType of an Account once
    // created).
    if (m_account_super_type == AccountSuperType::balance_sheet)
    {
        JEWEL_ASSERT (m_multi_account_panel);
        JEWEL_ASSERT (m_min_num_accounts >= 1);
        JEWEL_ASSERT (m_multi_account_panel->num_rows() >= 1);
        return true;
    }
    JEWEL_ASSERT (m_account_super_type == AccountSuperType::pl);
    AccountType const atypes[] =
    {   AccountType::revenue,
        AccountType::expense
    };
    for (AccountType atype: atypes)
    {
        if (!m_multi_account_panel->account_type_is_selected(atype))
        {
            error_message =
                wxString("You need at least one ") +
                account_type_to_string(atype) +
                wxString(" ") +
                account_concept_name(m_account_super_type) +
                wxString(" to start off with.");
            return false;
        }
    }
    return true;
}

void
SetupWizard::AccountPage::on_remove_checked_button_click(wxCommandEvent& event)
{
    (void)event;  // silence compiler re. unused variable
    JEWEL_ASSERT (m_multi_account_panel);
    m_multi_account_panel->remove_checked_rows();
    refresh_remove_checked_button_state();
    return;
}

void
SetupWizard::AccountPage::on_push_row_button_click(wxCommandEvent& event)
{
    (void)event;  // silence compiler re. unused variable
    JEWEL_ASSERT (m_multi_account_panel);
    m_multi_account_panel->push_row();
    refresh_remove_checked_button_state();
    return;
}

void
SetupWizard::AccountPage::on_wizard_page_changing
(   wxWizardEvent& event
)
{
    JEWEL_LOG_TRACE();
    wxString error_message;
    JEWEL_ASSERT (error_message.IsEmpty());
    if
    (   !account_names_valid(error_message) ||
        !account_types_valid(error_message)
    )
    {
        wxMessageBox(error_message);
        event.Veto();
        JEWEL_LOG_TRACE();
        return;
    }
    JEWEL_ASSERT (account_names_valid(error_message));
    JEWEL_ASSERT (account_types_valid(error_message));
    SetupWizard* p = dynamic_cast<SetupWizard*>(GetParent());
    JEWEL_ASSERT (p);
    JEWEL_ASSERT (m_multi_account_panel);
    p->set_account_names_already_taken
    (   m_multi_account_panel->selected_account_names()
    );
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::AccountPage::on_wizard_page_changed
(   wxWizardEvent& event
)
{
    JEWEL_LOG_TRACE();
    (void)event;  // silence compiler re. unused parameter
    JEWEL_ASSERT (m_multi_account_panel);
    m_multi_account_panel->update_summary();
    m_multi_account_panel->SetFocus();
    SetupWizard* p = dynamic_cast<SetupWizard*>(GetParent());
    JEWEL_ASSERT (p);
    m_multi_account_panel->
        set_account_names_already_taken(p->account_names_already_taken());
    JEWEL_LOG_TRACE();
    return;
}

void
SetupWizard::AccountPage::on_check_box_clicked(wxCommandEvent& event)
{
    JEWEL_LOG_TRACE();
    (void)event;  // silence compiler re. unused parameter
    refresh_remove_checked_button_state(); 
    JEWEL_LOG_TRACE();
}

wxString
SetupWizard::AccountPage::main_text() const
{
    wxString ret(" ");
    switch (m_account_super_type)
    {
    case AccountSuperType::balance_sheet:
        ret += wxString
        (   "Enter your assets (\"what you own\") and liabilities "
            "(\"what you owe\"), along with the current balances of "
            "each.\n\nAsset accounts normally have a positive balance, and "
            "liabilities normally have a negative balance."
        );
        break;
    case AccountSuperType::pl:
        ret += wxString
        (   "Enter some revenue and expenditure categories, along with "
            "the amount of funds you want to allocate to each of "
            "these \"envelopes\" to start out with."
        );
        break;
    default:
        JEWEL_HARD_ASSERT (false);
    }
    ret += wxString
    (   " \n\nNote, you can always add to or change these later, so it "
        "doesn't matter much if you can't think of them all now."
    );
    return ret;
}

}  // namespace gui
}  // namespace dcm


