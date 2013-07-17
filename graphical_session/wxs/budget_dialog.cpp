#include "budget_dialog.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
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
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "sizing.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/database_transaction.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::DatabaseTransaction;
using std::vector;

#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;

namespace phatbooks
{
namespace gui
{

// Begin event tables


BEGIN_EVENT_TABLE(BudgetDialog, wxDialog)
	EVT_BUTTON
	(	s_pop_item_button_id,
		BudgetDialog::on_pop_item_button_click
	)
	EVT_BUTTON
	(	s_push_item_button_id,
		BudgetDialog::on_push_item_button_click
	)
	EVT_BUTTON
	(	wxID_OK,
		BudgetDialog::on_ok_button_click
	)
	EVT_BUTTON
	(	wxID_CANCEL,
		BudgetDialog::on_cancel_button_click
	)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BudgetDialog::SpecialFrequencyCtrl, FrequencyCtrl)
	EVT_TEXT(wxID_ANY, BudgetDialog::SpecialFrequencyCtrl::on_text_change)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BudgetDialog::BalancingDialog, wxDialog)
	EVT_BUTTON(wxID_NO, BudgetDialog::BalancingDialog::on_no_button_click)
	EVT_BUTTON(wxID_YES, BudgetDialog::BalancingDialog::on_yes_button_click)
END_EVENT_TABLE()

// End event tables


BudgetDialog::BudgetDialog(Frame* p_parent, Account const& p_account):
	wxDialog(p_parent, wxID_ANY, wxEmptyString),
	m_next_row(0),
	m_top_sizer(0),
	m_summary_amount_text(0),
	m_summary_frequency_text(0),
	m_cancel_button(0),
	m_ok_button(0),
	m_account(p_account)
{
	assert (m_account.has_id());  // assert precondition
	assert (m_budget_items.empty());

	if (p_account == p_account.database_connection().balancing_account())
	{
		throw BudgetEditingException
		(	"Cannot use BudgetDialog to edit budgets for the budget "
			"balancing Account."
		);
	}
	assert (m_account != database_connection().balancing_account());

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);	

	// Row 0
	wxStaticText* account_label = new wxStaticText
	(	this,
		wxID_ANY,
		bstring_to_wx(m_account.name()),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	m_top_sizer->Add
	(	account_label,
		wxGBPosition(m_next_row, 1),
		wxGBSpan(1, 2),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
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
		wxGBPosition(m_next_row, 3),
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
		wxGBPosition(m_next_row, 4),
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
		wxGBPosition(m_next_row, 3),
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
		wxGBPosition(m_next_row, 4),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	
	++m_next_row;

	// Next row
	wxStaticText* description_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Description")
	);
	m_top_sizer->Add
	(	description_label,
		wxGBPosition(m_next_row, 1),
		wxGBSpan(1, 2)
	);
	wxStaticText* amount_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Amount")
	);
	m_top_sizer->Add
	(	amount_label,
		wxGBPosition(m_next_row, 3)
	);
	wxStaticText* frequency_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString("Frequency")
	);
	m_top_sizer->Add
	(	frequency_label,
		wxGBPosition(m_next_row, 4)
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

	// Final row
	m_cancel_button = new wxButton
	(	this,
		wxID_CANCEL,
		wxString("&Cancel"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y)
	);
	m_ok_button = new wxButton
	(	this,
		wxID_OK,
		wxString("&Save"),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y)
	);
	m_ok_button->SetDefault();  // Enter key will now trigger "Save" button

	add_bottom_row_widgets_to_sizer();

	// "Admin"
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Fit();
	CentreOnScreen();
	Layout();
}

void
BudgetDialog::on_pop_item_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter.
	pop_item_component();
	Fit();
	Layout();
	return;
}

void
BudgetDialog::on_push_item_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	BudgetItem budget_item(database_connection());
	budget_item.set_account(m_account);
	budget_item.set_description(BString(""));
	budget_item.set_amount(Decimal(0, m_account.commodity().precision()));
	budget_item.set_frequency(Frequency(1, interval_type::days));
	push_item_component(budget_item);
	Fit();
	Layout();
	return;
}

void
BudgetDialog::on_cancel_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	EndModal(wxID_CANCEL);
	return;
}

bool
BudgetDialog::TransferDataToWindow()
{
	if (!wxDialog::TransferDataToWindow())
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
	Decimal const zero(0, m_account.commodity().precision());

	for ( ; i != sz; ++i)
	{
		DecimalTextCtrl* const amount_ctrl =
			m_budget_item_components[i].amount_ctrl;
		Decimal const amount = amount_ctrl->amount();
		if
		(	((amount > zero) && (account_type == account_type::revenue)) ||
			((amount < zero) && (account_type == account_type::expense))
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
	// BudgetDialog.
	update_budget_summary();

	return true;
}

void
BudgetDialog::on_ok_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	if
	(	Validate() && TransferDataFromWindow() && update_budgets_from_dialog()
	)
	{
		prompt_to_balance();
		EndModal(wxID_OK);
	}
	return;
}

void
BudgetDialog::update_budget_summary()
{
	// WARNING This is inefficient.
	vector<BudgetItem> budget_items = make_budget_items();
	assert (m_summary_amount_text);
	vector<BudgetItem>::const_iterator it = budget_items.begin();
	if (budget_items.empty())
	{
		m_summary_amount_text->SetLabelText
		(	finformat_wx
			(	Decimal(0, m_account.commodity().precision()),
				locale()
			)
		);
	}
	else
	{
		assert (budget_items.end() - it > 0);
		m_summary_amount_text->SetLabelText
		(	finformat_wx
			(	normalized_total(it, budget_items.end()),
				locale()
			)
		);
	}
	return;
}

bool
BudgetDialog::update_budgets_from_dialog()
{
	DatabaseTransaction transaction(database_connection());

	typedef vector<BudgetItem> ItemVec;

	// Make m_budget_items match the BudgetItems implied by
	// m_budget_item_components (what is shown in the BudgetDialog).
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
	return true;
}

void
BudgetDialog::push_item_component(BudgetItem const& p_budget_item)
{
	assert (p_budget_item.account() == m_account);

	if (m_cancel_button)
	{
		assert (m_ok_button);
		detach_bottom_row_widgets_from_sizer();
	}

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
		wxGBPosition(m_next_row, 1),
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
		wxGBPosition(m_next_row, 3),
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
		wxGBPosition(m_next_row, 4),
		wxGBSpan(1, 2)
	);
	m_budget_item_components.push_back(budget_item_component);

	++m_next_row;

	if (m_cancel_button)
	{
		assert (m_ok_button);
		add_bottom_row_widgets_to_sizer();
		move_bottom_row_widgets_after_in_tab_order
		(	budget_item_component.frequency_ctrl
		);
	}

	return;
}

void
BudgetDialog::pop_item_component()
{
	assert (m_cancel_button);
	assert (m_ok_button);
	if (m_budget_item_components.empty())
	{
		return;
	}
	assert (m_budget_item_components.size() >= 1);
	detach_bottom_row_widgets_from_sizer();

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
	add_bottom_row_widgets_to_sizer();
	update_budget_summary();
	return;
}

wxString
BudgetDialog::initial_summary_amount_text()
{
	return finformat_wx(m_account.budget(), locale());
}

wxString
BudgetDialog::initial_summary_frequency_text()
{
	return
		std8_to_wx
		(	frequency_description
			(	database_connection().budget_frequency()
			)
		);
}

PhatbooksDatabaseConnection&
BudgetDialog::database_connection() const
{
	return m_account.database_connection();
}

void
BudgetDialog::detach_bottom_row_widgets_from_sizer()
{
	assert (m_cancel_button);
	assert (m_ok_button);
	m_top_sizer->Detach(m_cancel_button);
	m_top_sizer->Detach(m_ok_button);
	--m_next_row;
	return;
}

void
BudgetDialog::move_bottom_row_widgets_after_in_tab_order
(	wxWindow* p_tab_predecessor
)
{
	m_cancel_button->MoveAfterInTabOrder(p_tab_predecessor);
	m_ok_button->MoveAfterInTabOrder(m_cancel_button);
	return;
}

void
BudgetDialog::add_bottom_row_widgets_to_sizer()
{
	assert (m_cancel_button);
	assert (m_ok_button);
	m_top_sizer->Add(m_cancel_button, wxGBPosition(m_next_row, 1));
	m_top_sizer->Add(m_ok_button, wxGBPosition(m_next_row, 4));
	++m_next_row;
	return;
}

vector<BudgetItem>
BudgetDialog::make_budget_items() const
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
BudgetDialog::prompt_to_balance()
{
	Account const balancing_account =
		database_connection().balancing_account();
	Decimal const imbalance = balancing_account.budget();
	Decimal const zero(0, m_account.commodity().precision());
	if (imbalance == zero)
	{
		return;
	}
	if (!Account::no_user_pl_accounts_saved(database_connection()))
	{
		assert (imbalance != zero);
		account_type::AccountType const account_type =
			m_account.account_type();
		optional<Account> maybe_target_account;	
		if
		(	(   (account_type == account_type::expense) ||
			    (account_type == account_type::pure_envelope)    )
			      &&
			(   imbalance < zero   )
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
				if ((ra_it->budget() < zero) && (*ra_it != balancing_account))
				{
					maybe_target_account = *ra_it;
					goto out;
				}
			}
			for ( ; pe_it != pe_end; ++pe_it)
			{
				if ((pe_it->budget() < zero) && (*pe_it != balancing_account))
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

BudgetDialog::SpecialFrequencyCtrl::SpecialFrequencyCtrl
(	BudgetDialog* p_parent,
	wxWindowID p_id,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection
):
	FrequencyCtrl(p_parent, p_id, p_size, p_database_connection)
{
}

void
BudgetDialog::SpecialFrequencyCtrl::on_text_change(wxCommandEvent& event)
{
	(void)event;  // Silence compiler re. unused parameter.
	BudgetDialog* const budget_dialog =
		dynamic_cast<BudgetDialog*>(GetParent());
	assert (budget_dialog);
	budget_dialog->update_budget_summary();
	return;
}	

BudgetDialog::SignWarning::SignWarning
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
BudgetDialog::SignWarning::get_message
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

BudgetDialog::BalancingDialog::BalancingDialog
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
		wxString("&Offset to category below"),
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
BudgetDialog::BalancingDialog::on_no_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	EndModal(wxID_NO);
	return;
}

void
BudgetDialog::BalancingDialog::on_yes_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	update_budgets_from_dialog(m_account_ctrl->account());
	EndModal(wxID_YES);
	return;
}

void
BudgetDialog::BalancingDialog::update_budgets_from_dialog
(	Account const& p_target
)
{
	BString const offsetting_item_description("Offsetting budget adjustment");
	Frequency const target_frequency =
		m_database_connection.budget_frequency();
	BudgetItemReader reader(m_database_connection);
	BudgetItemReader::iterator it = reader.begin();
	BudgetItemReader::iterator const end = reader.end();
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
	assert (budget_is_balanced());
	return;
}

bool
BudgetDialog::BalancingDialog::budget_is_balanced() const
{
	return
		m_database_connection.balancing_account().budget() ==
		Decimal(0, 0);
}

}  // namespace gui
}  // namespace phatbooks
