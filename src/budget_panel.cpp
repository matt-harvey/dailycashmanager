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

#include "gui/budget_panel.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "budget_item.hpp"
#include "budget_item_table_iterator.hpp"
#include "commodity.hpp"
#include "finformat.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "dcm_database_connection.hpp"
#include "dcm_exceptions.hpp"
#include "string_flags.hpp"
#include "gui/account_ctrl.hpp"
#include "gui/account_dialog.hpp"
#include "gui/button.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/frame.hpp"
#include "gui/frequency_ctrl.hpp"
#include "gui/locale.hpp"
#include "gui/persistent_object_event.hpp"
#include "gui/sizing.hpp"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/decimal_exceptions.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <sqloxx/handle.hpp>
#include <wx/app.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>
#include <vector>

using boost::numeric_cast;
using boost::optional;
using jewel::Decimal;
using jewel::DecimalException;
using jewel::Log;
using jewel::value;
using sqloxx::DatabaseTransaction;
using sqloxx::Handle;
using std::vector;

namespace dcm
{
namespace gui
{

// Begin event tables

BEGIN_EVENT_TABLE(BudgetPanel, wxPanel)
	EVT_BUTTON
	(	s_pop_item_button_id,
		BudgetPanel::on_pop_item_button_click
	)
	EVT_BUTTON
	(	s_push_item_button_id,
		BudgetPanel::on_push_item_button_click
	)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BudgetPanel::SpecialFrequencyCtrl, FrequencyCtrl)
	EVT_TEXT(wxID_ANY, BudgetPanel::SpecialFrequencyCtrl::on_text_change)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BudgetPanel::BalancingDialog, wxDialog)
	EVT_BUTTON(wxID_NO, BudgetPanel::BalancingDialog::on_no_button_click)
	EVT_BUTTON(wxID_YES, BudgetPanel::BalancingDialog::on_yes_button_click)
END_EVENT_TABLE()

// End event tables

namespace
{
	int bad_code()
	{
		return numeric_cast<int>(wxID_HIGHEST + 1);
	}

}  // end anonymous namespace

BudgetPanel::BudgetPanel
(	AccountDialog* p_parent,
	Handle<Account> const& p_account
):
	wxPanel(p_parent, wxID_ANY),
	m_next_row(0),
	m_top_sizer(nullptr),
	m_summary_label(nullptr),
	m_summary_amount_text(nullptr),
	m_pop_item_button(nullptr),
	m_push_item_button(nullptr),
	m_account(p_account)
{
	JEWEL_ASSERT (p_parent);  // precondition
	JEWEL_ASSERT (m_budget_items.empty());

	if (p_account == p_account->database_connection().balancing_account())
	{
		JEWEL_THROW
		(	BudgetEditingException,
			"Cannot use BudgetPanel to edit budgets for the budget "
			"balancing Account."
		);
	}
	JEWEL_ASSERT (m_account != database_connection().balancing_account());

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	// Row 0
	JEWEL_ASSERT
	(	database_connection().budget_frequency() ==
		Frequency(1, IntervalType::days)
	);
	m_summary_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Daily top-up"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_summary_label,
		wxGBPosition(m_next_row, 0),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_summary_amount_text = new wxStaticText
	(	this,
		wxID_ANY,
		initial_summary_amount_text(),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_summary_amount_text,
		wxGBPosition(m_next_row, 1),
		wxDefaultSpan,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);

	++m_next_row;
	
	// Next row - dummy text used to fix sizing issue. Don't be tempted
	// to use non-wxDefaultSize on the other wxStaticText instances above
	// this, as this was causing alignment issues on Windows previously.
	wxStaticText* dummy_text = new wxStaticText
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	dummy_text,
		wxGBPosition(m_next_row, 0),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	++m_next_row;

	// Next row
	wxStaticText* budget_items_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("BUDGET ITEMS"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	budget_items_label,
		wxGBPosition(m_next_row, 0),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_pop_item_button = new Button
	(	this,
		s_pop_item_button_id,
		wxString("Remove item"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_pop_item_button,
		wxGBPosition(m_next_row, 2),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_push_item_button = new Button
	(	this,
		s_push_item_button_id,
		wxString("Add item"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_push_item_button,
		wxGBPosition(m_next_row, 3),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	
	++m_next_row;

	// Next row
	wxStaticText* description_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Description"),
		wxDefaultPosition,
		wxSize(large_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add
	(	description_label,
		wxGBPosition(m_next_row, 0),
		wxGBSpan(1, 2)
	);
	wxStaticText* amount_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Amount")
	);
	m_top_sizer->Add
	(	amount_label,
		wxGBPosition(m_next_row, 2)
	);
	wxStaticText* frequency_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Frequency")
	);
	m_top_sizer->Add
	(	frequency_label,
		wxGBPosition(m_next_row, 3)
	);

	++m_next_row;

	// Next row
	// Bare scope
	{
		BudgetItemTableIterator it(database_connection());
		BudgetItemTableIterator const end;
		for ( ; it != end; ++it)
		{
			if ((*it)->account() == m_account)
		 	{
			 	push_item_component(*it);
				m_budget_items.push_back(*it);
			}
		}
	}

	// "Admin"
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	GetParent()->Fit();

	// Not sure why this is necessary here...
	update_budget_summary(m_budget_items);
}

BudgetPanel::~BudgetPanel()
{
}

void
BudgetPanel::on_pop_item_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter.
	pop_item_component();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	GetParent()->Fit();
	return;
}

void
BudgetPanel::on_push_item_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	Handle<BudgetItem> const budget_item(database_connection());
	budget_item->set_account(m_account);
	budget_item->set_description(wxString(""));
	budget_item->set_amount(zero());
	budget_item->set_frequency(Frequency(1, IntervalType::days));
	push_item_component(budget_item);
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	GetParent()->Fit();
	return;
}

bool
BudgetPanel::TransferDataToWindow()
{
	if (!wxPanel::TransferDataToWindow())
	{
		return false;
	}
	// Make sure there are no unusual signs
	// (+ for revenue Accounts or - for expense Accounts) and warn the
	// user in case there are, giving them the opportunity to correct it.	
	auto const parent = dynamic_cast<AccountDialog*>(GetParent());
	JEWEL_ASSERT (parent);
	auto const account_type = parent->selected_account_type();
	Decimal const z = zero();
	auto sz = m_budget_item_components.size();
	vector<decltype(sz)> changed;
	for (decltype(sz) i = 0; i != sz; ++i)
	{
		auto& elem = m_budget_item_components[i];
		DecimalTextCtrl* const amount_ctrl = elem.amount_ctrl;
		Decimal const amount = amount_ctrl->amount();
		if
		(	((amount > z) && (account_type == AccountType::revenue)) ||
			((amount < z) && (account_type == AccountType::expense))
		)
		{
			SignWarning warning(this, account_type);
			int const result = warning.ShowModal();
			JEWEL_ASSERT ((result == wxID_YES) || (result == wxID_NO));
			if (result == wxID_YES)
			{
				// TODO MEDIUM PRIORITY There is a tiny possibility that
				// this will throw DecimalUnaryMinusException. If this
				// happens, the application will simply crash. This
				// is extremely unlikely ever to occur, but still,
				// crashing is not an acceptable reaction to this.
				amount_ctrl->set_amount(-amount);
				changed.push_back(i);
			}
		}
	}
	JEWEL_LOG_TRACE();

	// Update the budget summary text on the basis of what's now in the
	// BudgetPanel.
	try
	{
		update_budget_summary();
		update_budgets_from_dialog_without_saving();
		JEWEL_LOG_TRACE();
		return true;
	}
	catch (DecimalException&)
	{
		wxMessageBox
		(	"Cannot safely set this budget item to this amount or "
				"frequency; amount may be too large to process safely.",
			"Message",
			wxICON_EXCLAMATION | wxOK,
			this
		);
		JEWEL_LOG_TRACE();
		// Undo what we did to the amount controls (if anything)
		for (auto const j: changed)
		{
			auto& elem = m_budget_item_components[j];
			auto const amount = elem.amount_ctrl->amount();
			elem.amount_ctrl->set_amount(-amount);
		}
		JEWEL_LOG_TRACE();
		return false;
	}
}

bool
BudgetPanel::process_confirmation()
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_account->has_id());
	if (!Validate())
	{
		return false;
	}
	if (!TransferDataFromWindow())
	{
		return false;
	}
	try
	{
		update_budgets_from_dialog();
	}
	catch (jewel::Exception& e)
	{
		if
		(	dynamic_cast<DecimalException*>(&e) ||
			dynamic_cast<JournalOverflowException*>(&e)
		)
		{
			// TODO MEDIUM PRIORITY Handle extremely rare case where we
			// arrive here. This would be due to BudgetItem saving not
			// causing overflow in and of itself, but overflow
			// occurring when the budget instrument is regenerated, and
			// the total imbalance calculation causing overflow.
		}
		throw;
	}
	if (prompt_to_balance())
	{
		JEWEL_LOG_TRACE();
		JEWEL_ASSERT (m_account->has_id());
		return true;
	}
	else
	{
		m_account->ghostify();  // TODO LOW PRIORITY this is prob. unnecessary
		JEWEL_LOG_TRACE();
		return false;	
	}
}

void
BudgetPanel::revert_dialog_to_budgets()
{
	// TODO MEDIUM PRIORITY Make it deal with case where m_budget_items.size()
	// is not equal to m_budget_item_components.size()? (Though at time of
	// writing this is not called unless this is the case.)
	JEWEL_HARD_ASSERT
	(	m_budget_items.size() ==
		m_budget_item_components.size()
	);
	for (size_t i = 0; i != m_budget_item_components.size(); ++i)
	{
		Handle<BudgetItem> item = m_budget_items[i];
		BudgetItemComponent& component = m_budget_item_components[i];
		component.description_ctrl->SetValue(item->description());
		component.amount_ctrl->set_amount(item->amount());
		component.frequency_ctrl->set_frequency(item->frequency());
	}
	return;
}

void
BudgetPanel::update_budget_summary
(	vector<Handle<BudgetItem> > const& p_budget_items
)
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_summary_amount_text);
	Decimal new_total = zero();
	if (!p_budget_items.empty())
	{
		new_total =
			normalized_total(p_budget_items.begin(), p_budget_items.end());
	}
	m_summary_amount_text->SetLabelText
	(	finformat_wx
		(	new_total,
			locale(),
			DecimalFormatFlags().clear(string_flags::dash_for_zero)
		)
	);
	JEWEL_LOG_TRACE();
	return;
}

void
BudgetPanel::update_budget_summary()
{
	update_budget_summary(make_budget_items());
	return;
}

void
BudgetPanel::update_budgets_from_dialog_without_saving()
{
	typedef vector<Handle<BudgetItem> > ItemVec;

	// Make m_budget_items match the BudgetItems implied by
	// m_budget_item_components (what is shown in the BudgetPanel).
	ItemVec const items_new = make_budget_items();
	ItemVec::size_type const num_items_old = m_budget_items.size();
	ItemVec::size_type const num_items_new = items_new.size();
	ItemVec::size_type i = 0;
	for ( ; (i != num_items_old) && (i != num_items_new); ++i)
	{
		JEWEL_ASSERT (i < m_budget_items.size());
		JEWEL_ASSERT (i < m_budget_item_components.size());
		m_budget_items[i]->mimic(*(items_new[i]));
	}
	JEWEL_ASSERT ((i == num_items_old) || (i == num_items_new));
	if (num_items_old < num_items_new)
	{
		JEWEL_ASSERT (i == num_items_old);
		for ( ; i != num_items_new; ++i)
		{
			m_budget_items.push_back(items_new[i]);
		}
	}
	else
	{
		JEWEL_ASSERT (num_items_new <= num_items_old);
		JEWEL_ASSERT (num_items_old == m_budget_items.size());
		while (m_budget_items.size() != num_items_new)
		{
			JEWEL_ASSERT (m_budget_items.size() > num_items_new);	
			Handle<BudgetItem> const doomed_item = m_budget_items.back();
			m_budget_items.pop_back();
			doomed_item->remove();
		}
		JEWEL_ASSERT (m_budget_items.size() == num_items_new);
	}
	JEWEL_LOG_TRACE();
	return;
}

void
BudgetPanel::update_budgets_from_dialog()
{
	JEWEL_LOG_TRACE();
	JEWEL_ASSERT (m_account->has_id());
	DatabaseTransaction transaction(database_connection());
	update_budgets_from_dialog_without_saving();
	try
	{
		for (auto const& elem: m_budget_items) elem->save();
	}
	catch (std::exception&)
	{
		for (auto const& elem: m_budget_items) elem->ghostify();
		transaction.cancel();
		return;
	}
	transaction.commit();
	Frame* const frame = dynamic_cast<Frame*>(wxTheApp->GetTopWindow());
	JEWEL_ASSERT (frame);
	PersistentObjectEvent::fire
	(	frame,
		DCM_BUDGET_EDITED_EVENT,
		m_account->id()
	);
	JEWEL_LOG_TRACE();
	return;
}

void
BudgetPanel::push_item_component(Handle<BudgetItem> const& p_budget_item)
{
	JEWEL_ASSERT (p_budget_item->account() == m_account);
	BudgetItemComponent budget_item_component;
	budget_item_component.description_ctrl = new TextCtrl
	(	this,
		wxID_ANY,
		p_budget_item->description(),
		wxDefaultPosition,
		wxSize(large_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add
	(	budget_item_component.description_ctrl,
		wxGBPosition(m_next_row, 0),
		wxGBSpan(1, 2)
	);
	wxSize const desc_size =
		budget_item_component.description_ctrl->GetSize();
	Decimal const amount = p_budget_item->amount();
	budget_item_component.amount_ctrl = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), desc_size.y),
		amount.places(),
		false
	);
	budget_item_component.amount_ctrl->set_amount(amount);
	m_top_sizer->Add
	(	budget_item_component.amount_ctrl,
		wxGBPosition(m_next_row, 2),
		wxGBSpan(1, 1)
	);
	budget_item_component.frequency_ctrl = new SpecialFrequencyCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), desc_size.y),
		database_connection()
	);
	optional<Frequency> const maybe_frequency = p_budget_item->frequency();
	budget_item_component.frequency_ctrl->set_frequency(maybe_frequency);
	m_top_sizer->Add
	(	budget_item_component.frequency_ctrl,
		wxGBPosition(m_next_row, 3),
		wxGBSpan(1, 2)
	);
	m_budget_item_components.push_back(budget_item_component);
	++m_next_row;
	return;
}

void
BudgetPanel::pop_item_component()
{
	if (m_budget_item_components.empty())
	{
		return;
	}
	JEWEL_ASSERT (m_budget_item_components.size() >= 1);

	// Bare scope
	{
		BudgetItemComponent& last =
			m_budget_item_components[m_budget_item_components.size() - 1];
		m_top_sizer->Detach(last.description_ctrl);
		m_top_sizer->Detach(last.amount_ctrl);
		m_top_sizer->Detach(last.frequency_ctrl);
		last.description_ctrl->Destroy();
		last.description_ctrl = 0;
		last.amount_ctrl->Destroy();
		last.amount_ctrl = 0;
		last.frequency_ctrl->Destroy();
		last.frequency_ctrl = 0;
	}

	m_budget_item_components.pop_back();
	--m_next_row;
	update_budget_summary();
	return;
}

wxString
BudgetPanel::initial_summary_amount_text()
{
	return
		m_account->has_id()?
		finformat_wx(m_account->budget(), locale()):
		finformat_wx(zero(), locale());
}

DcmDatabaseConnection&
BudgetPanel::database_connection() const
{
	return m_account->database_connection();
}

Decimal
BudgetPanel::zero() const
{
	Handle<Commodity> const commodity =
	(	m_account->has_id()?
		m_account->commodity():
		database_connection().default_commodity()
	);
	return Decimal(0, commodity->precision());
}

vector<Handle<BudgetItem> >
BudgetPanel::make_budget_items() const
{
	vector<Handle<BudgetItem> > ret;
	for (BudgetItemComponent const& component: m_budget_item_components)
	{
		Handle<BudgetItem> const budget_item(database_connection());
		budget_item->set_account(m_account);
		budget_item->set_description(component.description_ctrl->GetValue());
		budget_item->set_amount(component.amount_ctrl->amount());
		JEWEL_ASSERT (component.frequency_ctrl->frequency());
		budget_item->set_frequency
		(	value(component.frequency_ctrl->frequency())
		);
		JEWEL_ASSERT
		(	database_connection().supports_budget_frequency
			(	budget_item->frequency()
			)
		);
		ret.push_back(budget_item);
	}
	JEWEL_ASSERT (ret.size() == m_budget_item_components.size());
	return ret;
}

bool
BudgetPanel::prompt_to_balance()
{
	JEWEL_LOG_TRACE();
	Handle<Account> const balancing_account =
		database_connection().balancing_account();
	Decimal const imbalance = balancing_account->budget();
	Decimal const z = zero();
	if (imbalance == z)
	{
		return true;
	}
	if (!Account::no_user_pl_accounts_saved(database_connection()))
	{
		JEWEL_ASSERT (imbalance != z);
		auto const account_type = m_account->account_type();
		optional<Handle<Account> > maybe_target_account;	
		if
		(	(account_type == AccountType::expense) ||
			(account_type == AccountType::pure_envelope)
		)
		{
			// If m_account is an expense or pure_envelope Account, then
			// usually we would expect the offset to go to a revenue Account,
			// or perhaps to a pure_envelope Account. Often all such offsets
			// will go to a single Account (e.g. "Salary"); so we look for
			// an revenue or pure_envelope Account that already has a
			// negative entry, and that is the Account we offer as
			// the "default" offsetting Account to the user.
			AccountTableIterator it(database_connection());
			AccountTableIterator const end;
			for ( ; it != end; ++it)
			{
				Handle<Account> const& acc = *it;
				AccountType const atype = acc->account_type();
				if
				(	(	(atype == AccountType::revenue) ||
						(atype == AccountType::pure_envelope)
					)
					&&
					(	acc->budget() < z
					) 
					&&
					(	acc != balancing_account
					)
				)
				{
					maybe_target_account = acc;
					break;
				}
			}
		}
		BalancingDialog balancing_dialog
		(	this,
			imbalance,
			maybe_target_account,
			database_connection()
		);
		if (balancing_dialog.ShowModal() == bad_code())
		{
			wxMessageBox
			(	"Cannot offset imbalance to the requested envelope. This "
					"is likely due to the amount of the imbalance being "
					"too large to process safely. Imbalance has been left"
					" as is.",
				"Message",
				wxICON_EXCLAMATION | wxOK,
				this
			);
			return false;
		}
	}
	return true;
}

BudgetPanel::SpecialFrequencyCtrl::SpecialFrequencyCtrl
(	BudgetPanel* p_parent,
	wxWindowID p_id,
	wxSize const& p_size,
	DcmDatabaseConnection& p_database_connection
):
	FrequencyCtrl(p_parent, p_id, p_size, p_database_connection)
{
}

void
BudgetPanel::SpecialFrequencyCtrl::on_text_change(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	BudgetPanel* const budget_panel =
		dynamic_cast<BudgetPanel*>(GetParent());
	JEWEL_ASSERT (budget_panel);
	if (!budget_panel->TransferDataToWindow())
	{
		budget_panel->revert_dialog_to_budgets();
	}
	return;
}

BudgetPanel::SignWarning::SignWarning
(	wxWindow* p_parent,
	AccountType p_account_type
):
	wxMessageDialog
	(	p_parent,
		get_message(p_account_type),
		wxEmptyString,
		wxYES_NO
	)
{
}

wxString
BudgetPanel::SignWarning::get_message
(	AccountType p_account_type
)
{
	JEWEL_ASSERT
	(	(p_account_type == AccountType::revenue) ||
		(p_account_type == AccountType::expense)
	);
	if (p_account_type == AccountType::revenue)
	{
		return wxString
		(	"Budget amounts for revenue categories should usually be "
			"negative. Do you want to change this amount to "
			"a negative number?"
		);
	}
	JEWEL_ASSERT (p_account_type == AccountType::expense);
	return wxString
	(	"Budget amounts for expense categories should usually be "
		"a positive. Do you want to change this amount to "
		"a positive number?"
	);
}

BudgetPanel::BalancingDialog::BalancingDialog
(	wxWindow* p_parent,
	jewel::Decimal const& p_imbalance,
	boost::optional<Handle<Account> > const& p_maybe_account,
	DcmDatabaseConnection& p_database_connection
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_top_sizer(nullptr),
	m_account_ctrl(nullptr),
	m_no_button(nullptr),
	m_yes_button(nullptr),
	m_imbalance(p_imbalance),
	m_database_connection(p_database_connection)
{
	JEWEL_LOG_TRACE();
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	int row = 0;

	wxString text("Budget is now out of balance by an amount of ");
	text += finformat_wx
	(	p_imbalance,
		locale(),
		DecimalFormatFlags().clear(string_flags::dash_for_zero)
	);
	text += wxString(".");
	wxStaticText* const imbalance_message = new wxStaticText
	(	this,
		wxID_ANY,
		text,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add(imbalance_message, wxGBPosition(row, 1), wxGBSpan(1, 2));

	++row;

	m_no_button = new Button
	(	this,
		wxID_NO,
		wxString("&Leave unbalanced"),
		wxDefaultPosition,
		wxSize(large_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add
	(	m_no_button,
		wxGBPosition(row, 1),
		wxDefaultSpan,
		wxALIGN_CENTRE
	);

	++row;

	m_yes_button = new Button
	(	this,
		wxID_YES,
		wxString("&Offset to ") +
			account_concept_name(AccountSuperType::pl) +
			wxString(" below"),
		wxDefaultPosition,
		wxSize(large_width(), wxDefaultSize.y)
	);
	m_top_sizer->Add
	(	m_yes_button,
		wxGBPosition(row, 1),
		wxDefaultSpan,
		wxALIGN_CENTRE
	);

	++row;

	m_account_ctrl = new AccountCtrl
	(	this,
		wxID_ANY,
		wxSize(large_width(), wxDefaultSize.y),
		account_types(AccountSuperType::pl),
		m_database_connection,
		true  // Exclude balancing Account (which would be useless)
	);
	if (p_maybe_account)
	{
		JEWEL_LOG_VALUE(Log::info, value(p_maybe_account)->name());
		m_account_ctrl->set_account(value(p_maybe_account));
	}
	m_top_sizer->Add
	(	m_account_ctrl,
		wxGBPosition(row, 1),
		wxDefaultSpan,
		wxALIGN_CENTRE
	);

	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
}

void
BudgetPanel::BalancingDialog::on_yes_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	JEWEL_ASSERT (m_account_ctrl);
	Handle<Account> account = m_account_ctrl->account();
	JEWEL_ASSERT (account->has_id());
	try
	{
		update_budgets_from_dialog(account);
		JEWEL_LOG_TRACE();
	}
	catch (std::exception&)
	{
		EndModal(bad_code());
		JEWEL_LOG_TRACE();
		return;
	}
	EndModal(wxID_OK);
	return;
}

void
BudgetPanel::BalancingDialog::on_no_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	EndModal(wxID_OK);
	return;
}

void
BudgetPanel::BalancingDialog::update_budgets_from_dialog
(	Handle<Account> const& p_target
)
{
	JEWEL_LOG_TRACE();
	wxString const offsetting_item_description("Offsetting budget adjustment");
	auto const target_frequency = m_database_connection.budget_frequency();
	auto const frame = dynamic_cast<Frame*>(wxTheApp->GetTopWindow());
	JEWEL_ASSERT (frame);

	// Copy first into a vector. (Uneasy about modifying database contents
	// while in the process of reading with a TableIterator.)
	vector<Handle<BudgetItem> > vec
	(	BudgetItemTableIterator(m_database_connection),
		(BudgetItemTableIterator())
	);
	JEWEL_LOG_TRACE();
	for (Handle<BudgetItem> const& elem: vec)
	{
		// If there is already a "general offsetting BudgetItem" for
		// the target Account, then roll it into that.
		if
		(	(elem->account() == p_target) &&
			(elem->description() == offsetting_item_description) &&
			(elem->frequency() == target_frequency)
		)
		{
			elem->set_amount(elem->amount() + m_imbalance);
			JEWEL_LOG_TRACE();
			elem->save();
			JEWEL_LOG_TRACE();
			JEWEL_ASSERT (budget_is_balanced());
			
			PersistentObjectEvent::fire
			(	frame,  // don't use "this", or event will be missed
				DCM_BUDGET_EDITED_EVENT,
				p_target->id()
			);
			JEWEL_LOG_TRACE();
			return;
		}
	}
	JEWEL_LOG_TRACE();

	// There was not already a "general offsetting BudgetItem" for
	// the target Account, so we create a new BudgetItem.
	Handle<BudgetItem> const adjusting_item(m_database_connection);
	adjusting_item->set_description(offsetting_item_description);
	adjusting_item->set_account(p_target);
	adjusting_item->set_frequency(target_frequency);
	adjusting_item->set_amount(m_imbalance);
	adjusting_item->save();
	JEWEL_LOG_TRACE();

	PersistentObjectEvent::fire
	(	frame,  // don't use "this", or event will be missed
		DCM_BUDGET_EDITED_EVENT,
		p_target->id()
	);
	JEWEL_ASSERT (budget_is_balanced());
	JEWEL_LOG_TRACE();
	return;
}

bool
BudgetPanel::BalancingDialog::budget_is_balanced() const
{
	return
		m_database_connection.balancing_account()->budget() ==
		Decimal(0, 0);
}

}  // namespace gui
}  // namespace dcm
