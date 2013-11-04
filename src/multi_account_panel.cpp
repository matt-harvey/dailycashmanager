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


#include "gui/multi_account_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "augmented_account.hpp"
#include "commodity.hpp"
#include "finformat.hpp"
#include "make_default_accounts.hpp"
#include "phatbooks_database_connection.hpp"
#include "string_flags.hpp"
#include "visibility.hpp"
#include "gui/account_type_ctrl.hpp"
#include "gui/decimal_text_ctrl.hpp"
#include "gui/gridded_scrolled_panel.hpp"
#include "gui/locale.hpp"
#include "gui/setup_wizard.hpp"
#include "gui/sizing.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <sqloxx/handle.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <numeric>
#include <set>
#include <vector>

using jewel::Decimal;
using jewel::Log;
using jewel::round;
using sqloxx::Handle;
using std::accumulate;
using std::set;
using std::vector;

// for debugging
	#include <jewel/log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{
namespace gui
{

namespace
{
	vector<Handle<Account> > suggested_accounts
	(	PhatbooksDatabaseConnection& p_database_connection,
		AccountSuperType p_account_super_type
	)
	{
		vector<Handle<Account> > ret;
		JEWEL_ASSERT (ret.empty());
		typedef vector<AccountType> ATypeVec;
		ATypeVec const& account_types =
			phatbooks::account_types(p_account_super_type);
		for (AccountType atype: account_types)
		{
			make_default_accounts(p_database_connection, ret, atype);
		}
		return ret;
	}

	Decimal total_amount_aux(Decimal const& dec, DecimalTextCtrl* ctrl)
	{
		return dec + ctrl->amount();
	}

}  // end anonymous namespace

MultiAccountPanel::MultiAccountPanel
(	SetupWizard::AccountPage* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	AccountSuperType p_account_super_type,
	Handle<Commodity> const& p_commodity,
	size_t p_minimum_num_rows
):
	GriddedScrolledPanel(p_parent, p_size, p_database_connection),
	m_account_super_type(p_account_super_type),
	m_commodity(p_commodity),
	m_summary_amount_text(nullptr),
	m_minimum_num_rows(p_minimum_num_rows)
{
	JEWEL_ASSERT (m_account_names_already_taken.empty());

	// Row of total text etc.
	wxString summary_label("Total");
	if (m_account_super_type == AccountSuperType::pl)
	{
		summary_label += wxString(" to allocate");
	}
	summary_label += ":";
	display_text(summary_label, 3, wxALIGN_RIGHT);
	m_summary_amount_text = display_decimal
	(	summary_amount(),
		4,
		false
	);
	// Dummy column to right
	display_text(wxEmptyString, 5);

	increment_row();
	increment_row();

	// Row of column headings
	AccountPhraseFlags const flags =
		AccountPhraseFlags().set(string_flags::capitalize);
	wxString const account_name_label =
		wxString(" ") +
		account_concept_name(m_account_super_type, flags) +
		wxString(" name:");
	display_text(account_name_label, 0);
	display_text(wxString(" Type:"), 1);
	display_text(wxString(" Description:"), 2);
	// Deliberately skipping column 3.
	display_text(wxString(" Starting balance:"), 4);

	increment_row();

	// Main body of MultiAccountPanel - a grid of fields where user
	// can edit Account attributes and opening balances.
	vector<Handle<Account> > sugg_accounts =
		suggested_accounts(database_connection(), m_account_super_type);
	auto const sz = sugg_accounts.size();
	m_account_name_boxes.reserve(sz);
	m_account_type_boxes.reserve(sz);
	m_description_boxes.reserve(sz);
	m_opening_balance_boxes.reserve(sz);
	for (auto const& account: sugg_accounts)
	{
		push_row(account);
	}
	while (num_rows() < m_minimum_num_rows)
	{
		push_row();
	}
	JEWEL_ASSERT (num_rows() >= p_minimum_num_rows);

	// "Admin"
	FitInside();
	// Layout();
}

MultiAccountPanel::~MultiAccountPanel()
{
}

int
MultiAccountPanel::required_width()
{
	return
		medium_width() * 3 +
		large_width() * 1 +
		standard_gap() * 3 +
		standard_border() * 2 +
		scrollbar_width_allowance();
}

bool
MultiAccountPanel::push_row()
{
	Handle<Account> account = blank_account();
	return push_row(account);
}

bool
MultiAccountPanel::pop_row()
{
	if (m_account_name_boxes.size() <= m_minimum_num_rows)
	{
		return false;
	}
	JEWEL_ASSERT (m_account_name_boxes.size() > 1);
#	ifndef NDEBUG
		vector<wxTextCtrl*>::size_type const sz = m_account_name_boxes.size();
		JEWEL_ASSERT (sz > 0);
		JEWEL_ASSERT (sz == m_account_type_boxes.size());
		JEWEL_ASSERT (sz == m_description_boxes.size());
		JEWEL_ASSERT (sz == m_opening_balance_boxes.size());
#	endif
	pop_widget_from(m_opening_balance_boxes);
	pop_widget_from(m_description_boxes);
	pop_widget_from(m_account_type_boxes);
	pop_widget_from(m_account_name_boxes);
	decrement_row();
	FitInside();
	return true;
}

Decimal
MultiAccountPanel::summary_amount() const
{
	if (m_account_super_type == AccountSuperType::balance_sheet)
	{
		return total_amount();
	}
	JEWEL_ASSERT (m_account_super_type == AccountSuperType::pl);
	SetupWizard::AccountPage const* const parent =
		dynamic_cast<SetupWizard::AccountPage const*>(GetParent());
	JEWEL_ASSERT (parent);
	return parent->total_balance_sheet_amount() - total_amount();
}

void
MultiAccountPanel::update_summary()
{
	m_summary_amount_text->SetLabel
	(	finformat_wx
		(	summary_amount(),
			locale(),
			DecimalFormatFlags().clear(string_flags::dash_for_zero)
		)
	);
	Layout();  // This is essential.
	return;
}

size_t
MultiAccountPanel::num_rows() const
{
	size_t const sz = m_account_name_boxes.size();
	JEWEL_ASSERT (sz == m_account_type_boxes.size());
	JEWEL_ASSERT (sz == m_description_boxes.size());
	JEWEL_ASSERT (sz == m_opening_balance_boxes.size());
	return m_account_name_boxes.size();
}

bool
MultiAccountPanel::account_type_is_selected
(	AccountType p_account_type
) const
{
	for (AccountTypeCtrl* const ctrl: m_account_type_boxes)
	{
		if (ctrl->account_type() == p_account_type)
		{
			return true;
		}
	}
	return false;
}

bool
MultiAccountPanel::TransferDataToWindow()
{
	if (!GriddedScrolledPanel::TransferDataToWindow())
	{
		return false;
	}
	update_summary();
	return true;
}

Handle<Account>
MultiAccountPanel::blank_account()
{
	Handle<Account> ret(database_connection());
	wxString const empty_string;
	JEWEL_ASSERT (empty_string.empty());
	ret->set_name(empty_string);
	ret->set_description(empty_string);
	ret->set_visibility(Visibility::visible);
	vector<AccountType> const& atypes =
		account_types(m_account_super_type);
	JEWEL_ASSERT (!atypes.empty());
	ret->set_account_type(atypes.at(0));
	return ret;
}

bool
MultiAccountPanel::push_row(Handle<Account> const& p_account)
{
	int const row = current_row();

	// Account name
	wxTextCtrl* account_name_box = new wxTextCtrl
	(	this,
		wxID_ANY,
		p_account->name(),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT
	);
	top_sizer().Add(account_name_box, wxGBPosition(row, 0));
	m_account_name_boxes.push_back(account_name_box);

	int const height = account_name_box->GetSize().GetY();

	// Account type
	AccountTypeCtrl* account_type_box = new AccountTypeCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), height),
		database_connection(),
		m_account_super_type
	);
	account_type_box->set_account_type(p_account->account_type());
	top_sizer().Add(account_type_box, wxGBPosition(row, 1));
	m_account_type_boxes.push_back(account_type_box);

	// Description
	wxTextCtrl* description_box = new wxTextCtrl
	(	this,
		wxID_ANY,
		p_account->description(),
		wxDefaultPosition,
		wxSize(large_width(), height),
		wxALIGN_LEFT
	);
	top_sizer().
		Add(description_box, wxGBPosition(row, 2), wxGBSpan(1, 2));
	m_description_boxes.push_back(description_box);

	p_account->set_commodity(m_commodity);

	// Opening balance
	DecimalTextCtrl* opening_balance_box = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), height),
		m_commodity->precision(),
		false
	);
	top_sizer().Add(opening_balance_box, wxGBPosition(row, 4));
	m_opening_balance_boxes.push_back(opening_balance_box);

	increment_row();

	FitInside();

	return true;
}

void
MultiAccountPanel::set_commodity(Handle<Commodity> const& p_commodity)
{
	m_commodity = p_commodity;
	Decimal::places_type const precision = p_commodity->precision();
	for (DecimalTextCtrl* ctrl: m_opening_balance_boxes)
	{
		// TODO MEDIUM PRIORITY Handle potential Decimal exception here on
		// rounding.
		Decimal const old_amount = ctrl->amount();
		Decimal const new_amount = round(old_amount, precision);
		ctrl->set_amount(new_amount);
	}
	return;
}

void
MultiAccountPanel::selected_augmented_accounts
(	vector<AugmentedAccount>& out
)
{
#	ifndef NDEBUG
		vector<AugmentedAccount>::size_type const original_size =
			out.size();
#	endif
	vector<AugmentedAccount>::size_type const sz =
		m_account_name_boxes.size();
	JEWEL_ASSERT (m_account_type_boxes.size() == sz);
	JEWEL_ASSERT (m_description_boxes.size() == sz);
	JEWEL_ASSERT (m_opening_balance_boxes.size() == sz);
	vector<AugmentedAccount>::size_type i = 0;
	for ( ; i != sz; ++i)
	{
		AugmentedAccount augmented_account
		(	database_connection(),
			m_commodity
		);
		Handle<Account> const& account = augmented_account.account;
		account->set_name(m_account_name_boxes[i]->GetValue().Trim());
		AccountType const account_type =
			m_account_type_boxes[i]->account_type();
		JEWEL_ASSERT (super_type(account_type) == m_account_super_type);
		account->set_account_type(account_type);
		account->set_description(m_description_boxes[i]->GetValue());
		account->set_visibility(Visibility::visible);
		account->set_commodity(m_commodity);
		augmented_account.technical_opening_balance =
		(	m_account_super_type == AccountSuperType::pl?
			-m_opening_balance_boxes[i]->amount():
			m_opening_balance_boxes[i]->amount()
		);
		JEWEL_ASSERT (!account->has_id());
		out.push_back(augmented_account);
	}
#	ifndef NDEBUG
		JEWEL_ASSERT (out.size() == original_size + sz);
#	endif
	return;
}

set<wxString>
MultiAccountPanel::selected_account_names() const
{
	set<wxString> ret;
	size_t const sz = m_account_name_boxes.size();
	for (size_t i = 0; i != sz; ++i)
	{
		ret.insert(m_account_name_boxes[i]->GetValue().Trim());	
	}
	return ret;
}

bool
MultiAccountPanel::account_names_valid(wxString& p_error_message) const
{
	JEWEL_LOG_TRACE();
	set<wxString> account_names;
	AccountPhraseFlags const flags =
		AccountPhraseFlags().set(string_flags::capitalize);
	for (wxTextCtrl* const box: m_account_name_boxes)
	{
		wxString const name = box->GetValue().Trim().Lower();
		if (name.IsEmpty())
		{
			p_error_message =
				account_concept_name(m_account_super_type, flags) +
				wxString(" name is blank");
			return false;
		}
		set<wxString> const* const account_name_sets[] =
			{ &account_names, &m_account_names_already_taken };
		for (auto account_name_set: account_name_sets)
		{
			JEWEL_LOG_TRACE();
			if (account_name_set->find(name) != account_name_set->end())
			{
				p_error_message = wxString("Duplicate name: ");
				p_error_message += name;
				return false;
			}
		}
		account_names.insert(name);
	}
	return true;
}

void
MultiAccountPanel::set_account_names_already_taken
(	set<wxString> const& p_account_names_already_taken
)
{
	for (auto name: p_account_names_already_taken)
	{
		m_account_names_already_taken.insert(name.Trim().Lower());
		JEWEL_LOG_VALUE(Log::info, name);
	}
	return;
}

Decimal
MultiAccountPanel::total_amount() const
{
	return accumulate
	(	m_opening_balance_boxes.begin(),
		m_opening_balance_boxes.end(),
		Decimal(0, m_commodity->precision()),
		total_amount_aux
	);
}

}  // namespace gui
}  // namespace phatbooks
