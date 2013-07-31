#include "multi_account_panel.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "decimal_text_ctrl.hpp"
#include "make_default_accounts.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <jewel/decimal.hpp>
#include <wx/gdicmn.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <cassert>
#include <set>
#include <vector>

using jewel::Decimal;
using jewel::round;
using std::set;
using std::vector;

namespace phatbooks
{
namespace gui
{

namespace
{
	vector<Account> const& suggested_accounts
	(	PhatbooksDatabaseConnection& p_database_connection,
		account_super_type::AccountSuperType p_account_super_type
	)
	{
		static bool initialized = false;
		static vector<Account> ret;
		if (!initialized)
		{
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
			initialized = true;
		}
		return ret;
	}

}  // end anonymous namespace


MultiAccountPanel::MultiAccountPanel
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type,
	Commodity const& p_commodity
):
	wxScrolledWindow
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		p_size,
		wxVSCROLL
	),
	m_account_super_type(p_account_super_type),
	m_current_row(0),
	m_top_sizer(0),
	m_database_connection(p_database_connection),
	m_commodity(p_commodity)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	// Row of column headings
	wxString account_name_label(" Account name:");
	wxString opening_balance_label(" Opening balance:");
	if (m_account_super_type == account_super_type::pl)
	{
		account_name_label = wxString(" Category name:");
		opening_balance_label = wxString(" Initial budget allocation");
	}

	make_text(account_name_label, 0);
	make_text(wxString(" Type:"), 1);
	make_text(wxString(" Description:"), 2);
	// Deliberately skipping column 3.
	make_text(opening_balance_label, 4);

	increment_row();

	// Main body of MultiAccountPanel - a grid of fields where user
	// can edit Account attributes and opening balances.
	vector<Account> sugg_accounts =
		suggested_accounts(m_database_connection, m_account_super_type);
	vector<Account>::size_type const sz = sugg_accounts.size();
	m_account_name_boxes.reserve(sz);
	m_account_type_boxes.reserve(sz);
	m_description_boxes.reserve(sz);
	m_opening_balance_boxes.reserve(sz);
	vector<Account>::iterator it = sugg_accounts.begin();
	vector<Account>::iterator const end = sugg_accounts.end();
	for ( ; it != end; ++it)
	{
		int const row = current_row();
	
		// Account name
		wxTextCtrl* account_name_box = new wxTextCtrl
		(	this,
			wxID_ANY,
			bstring_to_wx(it->name()),
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
		account_type_box->set_account_type(it->account_type());
		top_sizer().Add(account_type_box, wxGBPosition(row, 1));
		m_account_type_boxes.push_back(account_type_box);

		// Description
		wxTextCtrl* description_box = new wxTextCtrl
		(	this,
			wxID_ANY,
			it->description(),
			wxDefaultPosition,
			wxSize(large_width(), height),
			wxALIGN_LEFT
		);
		top_sizer().
			Add(description_box, wxGBPosition(row, 2), wxGBSpan(1, 2));
		m_description_boxes.push_back(description_box);

		it->set_commodity(m_commodity);

		// Opening balance
		DecimalTextCtrl* opening_balance_box = new DecimalTextCtrl
		(	this,
			wxID_ANY,
			wxSize(medium_width(), height),
			it->commodity().precision(),
			false
		);
		top_sizer().Add(opening_balance_box, wxGBPosition(row, 4));
		m_opening_balance_boxes.push_back(opening_balance_box);

		increment_row();
	}

	// "Admin"
	configure_scrollbars();
	Layout();
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

wxGridBagSizer&
MultiAccountPanel::top_sizer()
{
	assert (m_top_sizer);
	return *m_top_sizer;
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
			// TODO "Name" should be either "Account name" or "Category name".
			p_error_message = wxString("Name is blank");
			return false;
		}
		if (account_names.find(name) != account_names.end())
		{
			p_error_message = wxString("Duplicate account name: ");
			p_error_message += name;
			return false;
		}
	}
	return true;
}

int
MultiAccountPanel::current_row() const
{
	return m_current_row;
}

void
MultiAccountPanel::increment_row()
{
	++m_current_row;
}

void
MultiAccountPanel::make_text
(	wxString const& p_text,
	int p_column,
	int p_alignment_flags
)
{
	wxStaticText* header = new wxStaticText
	(	this,
		wxID_ANY,
		p_text,
		wxDefaultPosition,
		wxDefaultSize,
		p_alignment_flags
	);
	top_sizer().Add
	(	header,
		wxGBPosition(current_row(), p_column),
		wxDefaultSpan,
		p_alignment_flags
	);
	return;
}

PhatbooksDatabaseConnection&
MultiAccountPanel::database_connection()
{
	return m_database_connection;
}

PhatbooksDatabaseConnection const&
MultiAccountPanel::database_connection() const
{
	return m_database_connection;
}

void
MultiAccountPanel::configure_scrollbars()
{
	SetScrollRate(0, 10);
	FitInside();
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
