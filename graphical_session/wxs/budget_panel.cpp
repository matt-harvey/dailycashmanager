// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "budget_panel.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_dialog.hpp"
#include "account_reader.hpp"
#include "account_type.hpp"
#include "b_string.hpp"
#include "budget_item.hpp"
#include "budget_item_reader.hpp"
#include "decimal_text_ctrl.hpp"
#include "finformat.hpp"
#include "frame.hpp"
#include "frequency.hpp"
#include "frequency_ctrl.hpp"
#include "interval_type.hpp"
#include "locale.hpp"
#include "persistent_object_event.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "sizing.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <wx/app.h>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::DatabaseTransaction;
using std::vector;

// for debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
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


BudgetPanel::BudgetPanel(AccountDialog* p_parent, Account const& p_account):
	wxPanel(p_parent, wxID_ANY),
	m_next_row(0),
	m_top_sizer(0),
	m_summary_amount_text(0),
	m_summary_frequency_text(0),
	m_account(p_account)
{
	assert (p_parent);  // precondition
	assert (m_budget_items.empty());

	if (p_account == p_account.database_connection().balancing_account())
	{
		throw BudgetEditingException
		(	"Cannot use BudgetPanel to edit budgets for the budget "
			"balancing Account."
		);
	}
	assert (m_account != database_connection().balancing_account());

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	// Row 0
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
		wxGBPosition(m_next_row, 2),
		wxDefaultSpan,
		wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL
	);
	m_summary_frequency_text = new wxStaticText
	(	this,
		wxID_ANY,
		initial_summary_frequency_text(),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	m_summary_frequency_text,
		wxGBPosition(m_next_row, 3),
		wxGBSpan(1, 2),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);

	++m_next_row;
	
	// Next row - blank
	++m_next_row;

	// Next row
	m_pop_item_button = new wxButton
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
	m_push_item_button = new wxButton
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
		BudgetItemReader const reader(database_connection());
		BudgetItemReader::const_iterator it = reader.begin();
		BudgetItemReader::const_iterator const end = reader.end();
		for ( ; it != end; ++it)
		{
			if (it->account() == m_account)
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

	update_budget_summary();  // Not sure why this is necessary here.
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
	BudgetItem budget_item(database_connection());
	budget_item.set_account(m_account);
	budget_item.set_description(BString(""));
	budget_item.set_amount(zero());
	budget_item.set_frequency(Frequency(1, interval_type::days));
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
	vector<BudgetItemComponent>::size_type i = 0;
	vector<BudgetItemComponent>::size_type const sz =
		m_budget_item_components.size();
	account_type::AccountType const account_type = m_account.account_type();

	// Set precision of "zero" for more efficient comparisons.
	Decimal const z = zero();
	for ( ; i != sz; ++i)
	{
		DecimalTextCtrl* const amount_ctrl =
			m_budget_item_components[i].amount_ctrl;
		Decimal const amount = amount_ctrl->amount();
		if
		(	((amount > z) && (account_type == account_type::revenue)) ||
			((amount < z) && (account_type == account_type::expense))
		)
		{
			SignWarning warning(this, account_type);
			int const result = warning.ShowModal();
			assert ((result == wxID_YES) || (result == wxID_NO));
			if (result == wxID_YES)
			{
				// TODO Deal with tiny possibility of overflow here?
				amount_ctrl->set_amount(-amount);
			}
		}
	}

	// Update the budget summary text on the basis of what's now in the
	// BudgetPanel.
	update_budget_summary();

	return true;
}

bool
BudgetPanel::process_confirmation()
{
	assert (m_account.has_id());
	if
	(	Validate() && TransferDataFromWindow() && update_budgets_from_dialog()
	)
	{
		prompt_to_balance();
		return true;
	}
	return false;
}

void
BudgetPanel::update_budget_summary()
{
	// WARNING This is inefficient.
	assert (m_summary_amount_text);
	Decimal new_total = zero();
	vector<BudgetItem> budget_items = make_budget_items();
	if (!budget_items.empty())
	{
		new_total =
			normalized_total(budget_items.begin(), budget_items.end());
	}
	m_summary_amount_text->
		SetLabelText(finformat_wx(new_total, locale(), false));
	return;
}

bool
BudgetPanel::update_budgets_from_dialog()
{
	assert (m_account.has_id());
	DatabaseTransaction transaction(database_connection());

	typedef vector<BudgetItem> ItemVec;

	// Make m_budget_items match the BudgetItems implied by
	// m_budget_item_components (what is shown in the BudgetPanel).
	// Bare scope
	{
		ItemVec const items_new = make_budget_items();
		ItemVec::size_type const num_items_old = m_budget_items.size();
		ItemVec::size_type const num_items_new = items_new.size();
		ItemVec::size_type i = 0;
		for ( ; (i != num_items_old) && (i != num_items_new); ++i)
		{
			assert (i < m_budget_items.size());
			assert (i < m_budget_item_components.size());
			m_budget_items[i].mimic(items_new[i]);
		}
		assert ((i == num_items_old) || (i == num_items_new));
		if (num_items_old < num_items_new)
		{
			assert (i == num_items_old);
			for ( ; i != num_items_new; ++i)
			{
				m_budget_items.push_back(items_new[i]);
			}
		}
		else
		{
			assert (num_items_new <= num_items_old);
			assert (num_items_old == m_budget_items.size());
			while (m_budget_items.size() != num_items_new)
			{
				assert (m_budget_items.size() > num_items_new);	
				BudgetItem doomed_item = m_budget_items.back();
				m_budget_items.pop_back();
				doomed_item.remove();
			}
			assert (m_budget_items.size() == num_items_new);
		}
	}
	// Save the amended m_budget_items
	// Bare scope
	{
		ItemVec::iterator it = m_budget_items.begin();
		ItemVec::iterator const end = m_budget_items.end();
		for ( ; it != end; ++it) it->save();
	}

	transaction.commit();

	assert (m_account.has_id());

	Frame* const frame = dynamic_cast<Frame*>(wxTheApp->GetTopWindow());
	assert (frame);
	PersistentObjectEvent::fire
	(	frame,
		PHATBOOKS_BUDGET_EDITED_EVENT,
		m_account.id()
	);

	return true;
}

void
BudgetPanel::push_item_component(BudgetItem const& p_budget_item)
{
	assert (p_budget_item.account() == m_account);


	BudgetItemComponent budget_item_component = {0, 0, 0};
	budget_item_component.description_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		bstring_to_wx(p_budget_item.description()),
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
	Decimal const amount = p_budget_item.amount();
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
	optional<Frequency> const maybe_frequency = p_budget_item.frequency();
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
	assert (m_budget_item_components.size() >= 1);

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
		m_account.has_id()?
		finformat_wx(m_account.budget(), locale()):
		finformat_wx(zero(), locale());
}

wxString
BudgetPanel::initial_summary_frequency_text()
{
	return
		std8_to_wx
		(	frequency_description
			(	database_connection().budget_frequency()
			)
		);
}

PhatbooksDatabaseConnection&
BudgetPanel::database_connection() const
{
	return m_account.database_connection();
}

Decimal
BudgetPanel::zero() const
{
	Commodity const commodity =
	(	m_account.has_id()?
		m_account.commodity():
		database_connection().default_commodity()
	);
	return Decimal(0, commodity.precision());
}

vector<BudgetItem>
BudgetPanel::make_budget_items() const
{
	vector<BudgetItem> ret;
	vector<BudgetItem>::size_type i = 0;
	vector<BudgetItem>::size_type const sz = m_budget_item_components.size();
	for ( ; i != sz; ++i)
	{
		BudgetItemComponent const& component = m_budget_item_components.at(i);
		BudgetItem budget_item(database_connection());
		budget_item.set_account(m_account);
		budget_item.set_description(component.description_ctrl->GetValue());
		budget_item.set_amount(component.amount_ctrl->amount());
		assert (component.frequency_ctrl->frequency());
		budget_item.set_frequency
		(	value(component.frequency_ctrl->frequency())
		);
		assert
		(	database_connection().supports_budget_frequency
			(	budget_item.frequency()
			)
		);
		ret.push_back(budget_item);
	}
	assert (ret.size() == m_budget_item_components.size());
	return ret;
}

void
BudgetPanel::prompt_to_balance()
{
	Account const balancing_account =
		database_connection().balancing_account();
	Decimal const imbalance = balancing_account.budget();
	Decimal const z = zero();
	if (imbalance == z)
	{
		return;
	}
	if (!Account::no_user_pl_accounts_saved(database_connection()))
	{
		assert (imbalance != z);
		account_type::AccountType const account_type =
			m_account.account_type();
		optional<Account> maybe_target_account;	
		if
		(	(   (account_type == account_type::expense) ||
			    (account_type == account_type::pure_envelope)    )
			      &&
			(   imbalance < z  )
		)
		{
			// If m_account is an expense or pure_envelope Account, then
			// usually we would expect the offset to go to a revenue Account,
			// or perhaps to a pure_envelope Account. Often all such offsets
			// will go to a single Account (e.g. "Salary"); so we look for
			// an revenue or pure_envelope Account that already has a
			// negative entry, and that is the Account we offer as
			// the "default" offsetting Account to the user.
			RevenueAccountReader const ra_reader(database_connection());
			RevenueAccountReader::const_iterator ra_it =
				ra_reader.begin();
			RevenueAccountReader::const_iterator const ra_end =
				ra_reader.end();
			PureEnvelopeAccountReader const pe_reader(database_connection());
			PureEnvelopeAccountReader::const_iterator pe_it =
				pe_reader.begin();
			PureEnvelopeAccountReader::const_iterator const pe_end =
				pe_reader.end();
			for ( ; ra_it != ra_end; ++ra_it)
			{
				if ((ra_it->budget() < z) && (*ra_it != balancing_account))
				{
					maybe_target_account = *ra_it;
					goto out;
				}
			}
			for ( ; pe_it != pe_end; ++pe_it)
			{
				if ((pe_it->budget() < z) && (*pe_it != balancing_account))
				{
					maybe_target_account = *pe_it;
					goto out;
				}
			}
		}
		out:
		BalancingDialog balancing_dialog
		(	this,
			imbalance,
			maybe_target_account,
			database_connection()
		);
		// TODO Do we need to test return value below?
		balancing_dialog.ShowModal();
	}
	return;
}

BudgetPanel::SpecialFrequencyCtrl::SpecialFrequencyCtrl
(	BudgetPanel* p_parent,
	wxWindowID p_id,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
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
	assert (budget_panel);
	budget_panel->update_budget_summary();
	return;
}	

BudgetPanel::SignWarning::SignWarning
(	wxWindow* p_parent,
	account_type::AccountType p_account_type
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
(	account_type::AccountType p_account_type
)
{
	assert
	(	(p_account_type == account_type::revenue) ||
		(p_account_type == account_type::expense)
	);
	if (p_account_type == account_type::revenue)
	{
		return wxString
		(	"Budget amounts for revenue categories should usually be "
			"negative. Do you want to change this amount to "
			"a negative number?"
		);
	}
	assert (p_account_type == account_type::expense);
	return wxString
	(	"Budget amounts for expense categories should usually be "
		"a positive. Do you want to change this amount to "
		"a positive number?"
	);
}

BudgetPanel::BalancingDialog::BalancingDialog
(	wxWindow* p_parent,
	jewel::Decimal const& p_imbalance,
	boost::optional<Account> const& p_maybe_account,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_top_sizer(0),
	m_account_ctrl(0),
	m_no_button(0),
	m_yes_button(0),
	m_imbalance(p_imbalance),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	int row = 0;

	wxString text("Budget is now out of balance by an amount of ");
	text += finformat_wx(p_imbalance, locale(), false);
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

	m_no_button = new wxButton
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

	m_yes_button = new wxButton
	(	this,
		wxID_YES,
		wxString("&Offset to ") +
			bstring_to_wx(account_concept_name(account_super_type::pl)) +
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

	PLAccountReader account_reader(m_database_connection);
	assert (!account_reader.empty());
	Account const suggested_account =
	(	p_maybe_account?
		value(p_maybe_account):
		*account_reader.begin()
	);
	m_account_ctrl = new AccountCtrl
	(	this,
		wxID_ANY,
		suggested_account,
		wxSize(large_width(), wxDefaultSize.y),
		account_reader.begin(),
		account_reader.end(),
		true  // Exclude balancing Account (which would be useless)
	);
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
	assert (m_account_ctrl);
	Account account = m_account_ctrl->account();
	assert (account.has_id());
	update_budgets_from_dialog(account);
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
BudgetPanel::BalancingDialog::update_budgets_from_dialog(Account& p_target)
{
	BString const offsetting_item_description("Offsetting budget adjustment");
	Frequency const target_frequency =
		m_database_connection.budget_frequency();
	BudgetItemReader reader(m_database_connection);
	BudgetItemReader::iterator it = reader.begin();
	BudgetItemReader::iterator const end = reader.end();

	Frame* const frame = dynamic_cast<Frame*>(wxTheApp->GetTopWindow());
	assert (frame);

	for ( ; it != end; ++it)
	{
		// If there is already a "general offsetting BudgetItem" for
		// the target Account, then roll it into that.
		if
		(	(it->account() == p_target) &&
			(it->description() == offsetting_item_description) &&
			(it->frequency() == target_frequency)
		)
		{
			it->set_amount(it->amount() + m_imbalance);
			it->save();
			assert (budget_is_balanced());
			
			PersistentObjectEvent::fire
			(	frame,  // don't use "this", or event will be missed
				PHATBOOKS_BUDGET_EDITED_EVENT,
				p_target.id()
			);
			return;
		}
	}
	// There was not already a "general offsetting BudgetItem" for
	// the target Account, so we create a new BudgetItem.
	BudgetItem adjusting_item(m_database_connection);
	adjusting_item.set_description(offsetting_item_description);
	adjusting_item.set_account(p_target);
	adjusting_item.set_frequency(target_frequency);
	adjusting_item.set_amount(m_imbalance);
	adjusting_item.save();

	PersistentObjectEvent::fire
	(	frame,  // don't use "this", or event will be missed
		PHATBOOKS_BUDGET_EDITED_EVENT,
		p_target.id()
	);
	assert (budget_is_balanced());

	return;
}

bool
BudgetPanel::BalancingDialog::budget_is_balanced() const
{
	return
		m_database_connection.balancing_account().budget() ==
		Decimal(0, 0);
}

}  // namespace gui
}  // namespace phatbooks