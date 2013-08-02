#include "multi_account_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "decimal_text_ctrl.hpp"
#include "gridded_scrolled_panel.hpp"
#include "make_default_accounts.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <cassert>
#include <numeric>
#include <set>
#include <vector>

using jewel::Decimal;
using jewel::round;
using std::accumulate;
using std::set;
using std::vector;

// for debugging
	#include <jewel/debug_log.hpp>
	#include <iostream>
	using std::endl;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(MultiAccountPanel, GriddedScrolledPanel)
	EVT_BUTTON
	(	s_pop_row_button_id,
		MultiAccountPanel::on_pop_row_button_click
	)
	EVT_BUTTON
	(	s_push_row_button_id,
		MultiAccountPanel::on_push_row_button_click
	)
END_EVENT_TABLE()

namespace
{
	vector<Account> suggested_accounts
	(	PhatbooksDatabaseConnection& p_database_connection,
		account_super_type::AccountSuperType p_account_super_type
	)
	{
		vector<Account> ret;
		assert (ret.empty());
		typedef vector<account_type::AccountType> ATypeVec;
		ATypeVec const& account_types =
			phatbooks::account_types(p_account_super_type);
		ATypeVec::const_iterator it = account_types.begin();
		ATypeVec::const_iterator const end = account_types.end();
		for ( ; it != end; ++it)
		{
			make_default_accounts(p_database_connection, ret, *it);
		}
		return ret;
	}
	Decimal total_amount_aux(Decimal const& dec, DecimalTextCtrl* ctrl)
	{
		return dec + ctrl->amount();
	}

}  // end anonymous namespace

MultiAccountPanel::MultiAccountPanel
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type,
	Commodity const& p_commodity
):
	GriddedScrolledPanel(p_parent, p_size, p_database_connection),
	m_account_super_type(p_account_super_type),
	m_pop_row_button(0),
	m_push_row_button(0),
	m_commodity(p_commodity)
{
	// Buttons
	m_pop_row_button = new wxButton
	(	this,
		s_pop_row_button_id,
		wxString("Remove ") + account_concept_name(),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	top_sizer().Add
	(	m_pop_row_button,
		wxGBPosition(current_row(), 3),
		wxDefaultSpan,
		wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL
	);
	m_push_row_button = new wxButton
	(	this,
		s_push_row_button_id,
		wxString("Add ") + account_concept_name(),
		wxDefaultPosition,
		wxSize(medium_width(), wxDefaultSize.y),
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);
	top_sizer().Add
	(	m_push_row_button,
		wxGBPosition(current_row(), 4),
		wxDefaultSpan,
		wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL
	);

	increment_row();

	increment_row();

	// Row of column headings
	wxString const account_name_label =
		wxString(" ") + account_concept_name(true) + wxString(" name:");
	wxString opening_balance_label(" Opening balance:");
	if (m_account_super_type == account_super_type::pl)
	{
		opening_balance_label = wxString(" Initial budget allocation");
	}

	display_text(account_name_label, 0);
	display_text(wxString(" Type:"), 1);
	display_text(wxString(" Description:"), 2);
	// Deliberately skipping column 3.
	display_text(opening_balance_label, 4);

	increment_row();

	// Main body of MultiAccountPanel - a grid of fields where user
	// can edit Account attributes and opening balances.
	vector<Account> sugg_accounts =
		suggested_accounts(database_connection(), m_account_super_type);
	JEWEL_DEBUG_LOG_LOCATION;
	JEWEL_DEBUG_LOG << "m_account_super_type: " << m_account_super_type << endl;
	vector<Account>::size_type const sz = sugg_accounts.size();
	m_account_name_boxes.reserve(sz);
	m_account_type_boxes.reserve(sz);
	m_description_boxes.reserve(sz);
	m_opening_balance_boxes.reserve(sz);
	vector<Account>::iterator it = sugg_accounts.begin();
	vector<Account>::iterator const end = sugg_accounts.end();
	for ( ; it != end; ++it)
	{
		JEWEL_DEBUG_LOG << it->name() << endl;
		JEWEL_DEBUG_LOG_LOCATION;
		add_row(*it);
	}

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

Account
MultiAccountPanel::blank_account()
{
	Account ret(database_connection());
	BString const empty_string;
	assert (empty_string.empty());
	ret.set_name(empty_string);
	ret.set_description(empty_string);
	vector<account_type::AccountType> const& atypes =
		account_types(m_account_super_type);
	assert (!atypes.empty());
	JEWEL_DEBUG_LOG_LOCATION;
	ret.set_account_type(atypes.at(0));
	return ret;
}

void
MultiAccountPanel::add_row(Account& p_account)
{
	JEWEL_DEBUG_LOG << "Entered MultiAccountPanel::add_row(...)" << endl;
	
	int const row = current_row();

	// Account name
	wxTextCtrl* account_name_box = new wxTextCtrl
	(	this,
		wxID_ANY,
		bstring_to_wx(p_account.name()),
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
	JEWEL_DEBUG_LOG_LOCATION;
	JEWEL_DEBUG_LOG << "p_account.name(): "
	                << p_account.name()
					<< endl;
	JEWEL_DEBUG_LOG << "p_account.account_type(): "
	                << account_type_to_string(p_account.account_type())
					<< endl;
	account_type_box->set_account_type(p_account.account_type());
	top_sizer().Add(account_type_box, wxGBPosition(row, 1));
	m_account_type_boxes.push_back(account_type_box);

	// Description
	wxTextCtrl* description_box = new wxTextCtrl
	(	this,
		wxID_ANY,
		p_account.description(),
		wxDefaultPosition,
		wxSize(large_width(), height),
		wxALIGN_LEFT
	);
	top_sizer().
		Add(description_box, wxGBPosition(row, 2), wxGBSpan(1, 2));
	m_description_boxes.push_back(description_box);

	p_account.set_commodity(m_commodity);

	// Opening balance
	DecimalTextCtrl* opening_balance_box = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		wxSize(medium_width(), height),
		p_account.commodity().precision(),
		false
	);
	top_sizer().Add(opening_balance_box, wxGBPosition(row, 4));
	m_opening_balance_boxes.push_back(opening_balance_box);

	increment_row();

	FitInside();

	return;
}

void
MultiAccountPanel::set_commodity(Commodity const& p_commodity)
{
	Decimal::places_type const precision = p_commodity.precision();
	if (precision == m_commodity.precision())
	{
		return;
	}
	assert (precision != m_commodity.precision());
	vector<DecimalTextCtrl*>::size_type i = 0;
	vector<DecimalTextCtrl*>::size_type const sz =
		m_opening_balance_boxes.size();
	for ( ; i != sz; ++i)
	{
		// TODO Handle potential Decimal exception here on rounding.
		Decimal const old_amount = m_opening_balance_boxes[i]->amount();
		Decimal const new_amount = round(old_amount, precision);
		m_opening_balance_boxes[i]->set_amount(new_amount);
	}
	return;
}

void
MultiAccountPanel::selected_augmented_accounts
(	vector<AugmentedAccount>& out
)
{
	assert (out.empty());  // precondition

	vector<AugmentedAccount>::size_type const sz =
		m_account_name_boxes.size();
	assert (m_account_type_boxes.size() == sz);
	assert (m_description_boxes.size() == sz);
	assert (m_opening_balance_boxes.size() == sz);
	vector<AugmentedAccount>::size_type i = 0;
	for ( ; i != sz; ++i)
	{
		AugmentedAccount augmented_account
		(	database_connection(),
			m_commodity
		);
		Account& account = augmented_account.account;
		account.set_name
		(	wx_to_bstring(m_account_name_boxes[i]->GetValue().Trim())
		);
		account_type::AccountType const account_type =
			m_account_type_boxes[i]->account_type();
		assert (super_type(account_type) == m_account_super_type);
		JEWEL_DEBUG_LOG_LOCATION;
		account.set_account_type(account_type);
		account.set_description(m_description_boxes[i]->GetValue());
		account.set_commodity(m_commodity);

		// TODO Make sure it is clear to the user which way round the
		// signs are supposed to go.
		augmented_account.technical_opening_balance =
			m_opening_balance_boxes[i]->amount();

		assert (!account.has_id());
		out.push_back(augmented_account);
	}
	assert (out.size() == sz);
	return;
}

bool
MultiAccountPanel::account_names_valid(wxString& p_error_message) const
{
	set<wxString> account_names;
	vector<wxTextCtrl*>::size_type i = 0;
	vector<wxTextCtrl*>::size_type const sz = m_account_name_boxes.size();
	for ( ; i != sz; ++i)
	{
		wxString const name =
			m_account_name_boxes[i]->GetValue().Trim().Lower();
		if (name.IsEmpty())
		{
			p_error_message =
				account_concept_name(true) + wxString(" name is blank");
			return false;
		}
		if (account_names.find(name) != account_names.end())
		{
			p_error_message = wxString("Duplicate account name: ");
			p_error_message += name;
			return false;
		}
		account_names.insert(name);
	}
	return true;
}

Decimal
MultiAccountPanel::total_amount() const
{
	return accumulate
	(	m_opening_balance_boxes.begin(),
		m_opening_balance_boxes.end(),
		Decimal(0, m_commodity.precision()),
		total_amount_aux
	);
}

wxString
MultiAccountPanel::account_concept_name(bool p_capitalize) const
{
	// TODO This "user facing Account concept name" concept is used
	// elsewhere. This information should be singly located for use
	// across the application - probably in a free standing function in
	// "account.hpp".
	switch (m_account_super_type)
	{
	case account_super_type::balance_sheet:
		return p_capitalize? wxString("Account"): wxString("account");
	case account_super_type::pl:
		return p_capitalize? wxString("Category"): wxString("category");
	default:
		assert (false);
	}
	assert (false);
}

void
MultiAccountPanel::on_pop_row_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused variable
	if (m_account_name_boxes.empty())
	{
		return;
	}
#	ifndef NDEBUG
		vector<wxTextCtrl*>::size_type const sz = m_account_name_boxes.size();
		assert (sz > 0);
		assert (sz == m_account_type_boxes.size());
		assert (sz == m_description_boxes.size());
		assert (sz == m_opening_balance_boxes.size());
#	endif

	pop_widget_from(m_opening_balance_boxes);
	pop_widget_from(m_description_boxes);
	pop_widget_from(m_account_type_boxes);
	pop_widget_from(m_account_name_boxes);

	decrement_row();

	FitInside();
	return;
}

void
MultiAccountPanel::on_push_row_button_click(wxCommandEvent& event)
{
	(void)event;  // silence compiler re. unused variable
	Account account = blank_account();
	JEWEL_DEBUG_LOG_LOCATION;
	add_row(account);
	return;
}

MultiAccountPanel::AugmentedAccount::AugmentedAccount
(	PhatbooksDatabaseConnection& p_database_connection,
	Commodity const& p_commodity
):
	account(p_database_connection),
	technical_opening_balance(0, p_commodity.precision())
{
}

MultiAccountPanel::AugmentedAccount::AugmentedAccount
(	Account const& p_account,
	Decimal const& p_technical_opening_balance
):
	account(p_account),
	technical_opening_balance(p_technical_opening_balance)
{
}


}  // namespace gui
}  // namespace phatbooks
