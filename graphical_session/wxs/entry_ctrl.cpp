// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "entry_ctrl.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "decimal_text_ctrl.hpp"
#include "entry.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "transaction_type.hpp"
#include "transaction_ctrl.hpp"
#include "sizing.hpp"
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <cassert>
#include <vector>

using boost::optional;
using boost::scoped_ptr;
using jewel::Decimal;
using jewel::value;
using std::vector;

#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;

namespace phatbooks
{
namespace gui
{

BEGIN_EVENT_TABLE(EntryCtrl, wxPanel)
	EVT_BUTTON
	(	s_split_button_id,
		EntryCtrl::on_split_button_click
	)
END_EVENT_TABLE()


EntryCtrl::EntryCtrl
(	TransactionCtrl* p_parent,
	vector<Account> const& p_accounts,
	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type,
	wxSize const& p_text_ctrl_size,
	bool p_is_source
):
	wxPanel(p_parent),
	m_database_connection(p_database_connection),
	m_is_source(p_is_source),
	m_transaction_type(p_transaction_type),
	m_account_reader(0),
	m_text_ctrl_size(p_text_ctrl_size),
	m_top_sizer(0),
	m_side_descriptor(0),
	m_next_row(0)
{
	assert (m_account_name_boxes.empty());
	assert (m_comment_boxes.empty());
	assert (m_amount_boxes.empty());
	assert_transaction_type_validity(m_transaction_type);

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	bool const multiple_entries = (p_accounts.size() > 1);

	// Row 0
	configure_top_row(multiple_entries);

	// Subsequent rows
	configure_account_reader();
	vector<Account>::const_iterator it = p_accounts.begin();
	vector<Account>::const_iterator const end = p_accounts.end();
	optional<Decimal> const maybe_previous_row_amount;
	for ( ; it != end; ++it)
	{
		add_row
		(	*it,
			wxEmptyString,
			Decimal(0, it->commodity().precision()),
			maybe_previous_row_amount,
			false,
			multiple_entries
		);
	}
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	// Fit();
	// Layout();
	GetParent()->Fit();
}

EntryCtrl::EntryCtrl
(	TransactionCtrl* p_parent,
	std::vector<Entry> const& p_entries,
	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type,
	wxSize const& p_text_ctrl_size,
	bool p_is_source
):
	wxPanel(p_parent),
	m_database_connection(p_database_connection),
	m_is_source(p_is_source),
	m_transaction_type(p_transaction_type),
	m_account_reader(0),
	m_text_ctrl_size(p_text_ctrl_size),
	m_top_sizer(0),
	m_side_descriptor(0),
	m_next_row(0)
{
	assert (m_account_name_boxes.empty());
	assert (m_comment_boxes.empty());
	assert (m_amount_boxes.empty());
	assert_transaction_type_validity(m_transaction_type);

	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	vector<Entry>::size_type const sz = p_entries.size();
	bool const multiple_entries = (sz > 1);

	// Row 0
	configure_top_row(multiple_entries);

	// Subsequent rows

	configure_account_reader();
	optional<Decimal> maybe_previous_row_amount;
	for (vector<Entry>::size_type i = 0; i != sz; ++i)
	{
		Entry const& entry = p_entries[i];
		Decimal amount = (m_is_source? -entry.amount(): entry.amount());
		if (m_transaction_type == transaction_type::envelope_transaction)
		{
			amount = -amount;
		}
		add_row
		(	entry.account(),
			bstring_to_wx(entry.comment()),
			amount,
			entry.is_reconciled(),
			maybe_previous_row_amount,
			multiple_entries
		);
		if (i == 0) maybe_previous_row_amount = amount;
	}
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	// Fit();
	// Layout();
	GetParent()->Fit();
}

EntryCtrl::~EntryCtrl()
{
	delete m_account_reader;
	m_account_reader = 0;
}

void
EntryCtrl::configure_account_reader()
{
	if (m_is_source)
	{
		assert (!m_account_reader);
		m_account_reader = create_source_account_reader
		(	m_database_connection,
			m_transaction_type
		);
	}
	else
	{
		assert (!m_account_reader);
		m_account_reader = create_destination_account_reader
		(	m_database_connection,
			m_transaction_type
		);
	}
	return;
}

void
EntryCtrl::configure_top_row(bool p_include_split_button)
{
	assert (m_next_row == 0);
	m_side_descriptor = new wxStaticText(this, wxID_ANY, side_description());
	m_top_sizer->Add(m_side_descriptor, wxGBPosition(m_next_row, 0));

	wxStaticText* const comment_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString(" Memo:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(comment_label, wxGBPosition(m_next_row, 1));

	// Split button is in row 0 if and only if there are multiple
	// Accounts.
	if (p_include_split_button)
	{
		// Note the order of construction of elements effects tab
		// traversal.
		m_split_button = new wxButton
		(	this,
			s_split_button_id,
			wxString("Split"),
			wxDefaultPosition,
			m_text_ctrl_size
		);
		m_top_sizer->Add(m_split_button, wxGBPosition(m_next_row, 3));
	}
	++m_next_row;
	assert (m_next_row == 1);
	return;
}

void
EntryCtrl::refresh_for_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	assert_transaction_type_validity(p_transaction_type);
	if (p_transaction_type == m_transaction_type)
	{
		// Do nothing
		return;
	}
	assert (p_transaction_type != m_transaction_type);
	m_transaction_type = p_transaction_type;

	delete m_account_reader;
	m_account_reader = 0;

	if (m_is_source)
	{
		assert (!m_account_reader);
		m_account_reader = create_source_account_reader
		(	m_database_connection,
			m_transaction_type
		);
	}
	else
	{
		assert (!m_account_reader);
		m_account_reader = create_destination_account_reader
		(	m_database_connection,
			m_transaction_type
		);
	}
	for
	(	vector<AccountCtrl*>::size_type i = 0;
		i != m_account_name_boxes.size();
		++i
	)
	{
		assert (m_account_reader);
		m_account_name_boxes[i]->
			set(m_account_reader->begin(), m_account_reader->end());
	}
	return;
}

Decimal
EntryCtrl::primary_amount() const
{
	TransactionCtrl const* const parent =
		dynamic_cast<TransactionCtrl const*>(GetParent());
	assert (parent);
	return parent->primary_amount();
}

bool
EntryCtrl::is_balanced() const
{
	return total_amount() == primary_amount();
}

vector<Entry>
EntryCtrl::make_entries() const
{
	assert (m_account_name_boxes.size() == m_comment_boxes.size());
	assert (m_comment_boxes.size() >= m_amount_boxes.size());
	assert (m_amount_boxes.size() <= m_account_name_boxes.size());
	typedef std::vector<Entry>::size_type Size;
	Size const sz = m_account_name_boxes.size();

	vector<Entry> ret;
	for (Size i = 0; i != sz; ++i)
	{
		Entry entry(m_database_connection);

		assert (m_account_name_boxes[i]);
		entry.set_account(m_account_name_boxes[i]->account());

		assert (m_comment_boxes[i]);
		entry.set_comment(wx_to_bstring(m_comment_boxes[i]->GetValue()));

		Decimal amount = primary_amount();
		if (m_amount_boxes.size() != 0)
		{
			assert (m_amount_boxes.size() == sz);
			assert (sz > 1);
			assert (m_amount_boxes[i]);
			amount = m_amount_boxes[i]->amount();
		}
		if (!transaction_type_is_actual(m_transaction_type))
		{
			amount = -amount;
		}
		if (m_is_source)
		{
			amount = -amount;
		}
		entry.set_amount(amount);
		entry.set_whether_reconciled(m_reconciliation_statuses[i]);
		ret.push_back(entry);
		assert (!entry.has_id());
	}
	assert (ret.size() == sz);
	return ret;
}

bool
EntryCtrl::is_all_zero() const
{
	if (m_amount_boxes.empty())
	{
		// WARNING Tightly coupled!
		TransactionCtrl const* const transaction_ctrl =
			dynamic_cast<TransactionCtrl const*>(GetParent());
		assert (transaction_ctrl);
		return transaction_ctrl->primary_amount() == Decimal(0, 0);
	}
	for
	(	vector<EntryDecimalTextCtrl*>::size_type i = 0;
		i != m_amount_boxes.size();
		++i
	)
	{
		if (m_amount_boxes[i]->amount() != Decimal(0, 0))
		{
			return false;
		}
	}
	return true;
}

Decimal
EntryCtrl::total_amount() const
{
	if (m_amount_boxes.empty())
	{
		return primary_amount();
	}
	vector<EntryDecimalTextCtrl*>::size_type const sz = m_amount_boxes.size();
	assert (sz >= 2);
	Decimal ret(0, 0);
	for (vector<EntryDecimalTextCtrl*>::size_type i = 0; i != sz; ++i)
	{
		ret += m_amount_boxes[i]->amount();
	}
	return ret;
}

void
EntryCtrl::on_split_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	Account const account =
		m_account_name_boxes.at(m_account_name_boxes.size() - 1)->account();
	Decimal const amount(0, account.commodity().precision());
	optional<Decimal> const maybe_prev_amount;
	add_row(account, wxEmptyString, amount, false, maybe_prev_amount, true);

	return;
}

void
EntryCtrl::add_row
(	Account const& p_account,
	wxString const& p_comment,
	Decimal const& p_amount,
	bool p_is_reconciled,
	optional<Decimal> const& p_previous_row_amount,
	bool p_multiple_entries
)
{
	AccountCtrl* account_name_box = new AccountCtrl
	(	this,
		wxID_ANY,
		p_account,
		m_text_ctrl_size,
		m_account_reader->begin(),
		m_account_reader->end()
	);
	m_top_sizer->Add(account_name_box, wxGBPosition(m_next_row, 0));
	m_account_name_boxes.push_back(account_name_box);
	wxTextCtrl* comment_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		p_comment,
		wxDefaultPosition,
		wxSize(m_text_ctrl_size.x * 2 + standard_gap(), m_text_ctrl_size.y),
		wxALIGN_LEFT
	);
	m_top_sizer->
		Add(comment_ctrl, wxGBPosition(m_next_row, 1), wxGBSpan(1, 2));
	m_comment_boxes.push_back(comment_ctrl);

	m_reconciliation_statuses.push_back(static_cast<int>(p_is_reconciled));

	// If there is only one Account then there is only one "Entry line",
	// and that Entry line will also house the "Split" button. There will
	// then be no need to have an amount_ctrl for just the one
	// Entry. If there are multiple Entries then we must have already
	// placed the Split button at row 0, and in that case we also need
	// an amount_ctrl for each Entry.
	if (!p_multiple_entries)
	{
		m_split_button = new wxButton
		(	this,
			s_split_button_id,
			wxString("Split"),
			wxDefaultPosition,
			m_text_ctrl_size
		);
		m_top_sizer->Add(m_split_button, wxGBPosition(m_next_row, 3));
	}
	else
	{
		assert (p_multiple_entries);
		EntryDecimalTextCtrl* amount_ctrl = new EntryDecimalTextCtrl
		(	this,
			m_text_ctrl_size
		);
		if (m_amount_boxes.empty() && (m_next_row == 2))
		{
			// Then this is either the 0 or the 1 entry row. The 0 one
			// doesn't have an amount box...
			// The 0 Entry line now needs a EntryDecimalTextCtrl too, and
			// we need to reposition m_split_button to make way for it.
			m_top_sizer->Detach(m_split_button);
			m_top_sizer->Add(m_split_button, wxGBPosition(0, 3));
			assert (!m_account_name_boxes.empty());
			m_split_button->MoveBeforeInTabOrder(m_account_name_boxes[0]);
			EntryDecimalTextCtrl* prev_amount_ctrl = new EntryDecimalTextCtrl
			(	this,
				m_text_ctrl_size
			);
			if (p_previous_row_amount)
			{
				prev_amount_ctrl->set_amount(value(p_previous_row_amount));
			}
			else
			{
				prev_amount_ctrl->set_amount(primary_amount());
			}
			m_top_sizer->
				Add(prev_amount_ctrl, wxGBPosition(1, 3));
			prev_amount_ctrl->MoveBeforeInTabOrder(account_name_box);
			assert (m_amount_boxes.empty());
			m_amount_boxes.push_back(prev_amount_ctrl);
		}
		amount_ctrl->set_amount(p_amount);
		m_top_sizer->Add(amount_ctrl, wxGBPosition(m_next_row, 3));
		m_amount_boxes.push_back(amount_ctrl);
	}

	++m_next_row;

	Layout();  // Must call this.
	
	TransactionCtrl* parent = dynamic_cast<TransactionCtrl*>(GetParent());
	assert (parent);

	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	// GetParent()->Layout(); // Do not call this.
	// GetParent()->GetSizer()->RecalcSizes();  // Do not call this.
	parent->FitInside();
	// parent->Fit();  // Do not call this.

	return;
}

wxString
EntryCtrl::side_description() const
{
	if (m_is_source)
	{
		return wxString(" Source:");
	}
	assert (!m_is_source);
	return wxString(" Destination:");
}

void
EntryCtrl::autobalance(EntryDecimalTextCtrl* p_target)
{
	assert (p_target);
	Decimal const orig_total = total_amount();
	Decimal const orig_primary_amount = primary_amount();
	if (orig_total != orig_primary_amount)
	{
		Decimal const orig_target_amount = p_target->amount();
		Decimal const required_increment = orig_primary_amount - orig_total;
		p_target->set_amount(orig_target_amount + required_increment);
		assert (p_target->amount().places() == orig_target_amount.places());
	}

	// Postconditions
	assert (primary_amount() == orig_primary_amount);
	assert (total_amount() == primary_amount());
	return;
}

EntryCtrl::EntryDecimalTextCtrl::EntryDecimalTextCtrl
(	EntryCtrl* p_parent,
	wxSize const& p_size
):
	DecimalTextCtrl
	(	p_parent,
		wxID_ANY,
		p_size,
		p_parent->primary_amount().places(),
		false
	)
{
}

EntryCtrl::EntryDecimalTextCtrl::~EntryDecimalTextCtrl()
{
}

void
EntryCtrl::EntryDecimalTextCtrl::do_on_kill_focus(wxFocusEvent& event)
{
	GetParent()->Validate();
	GetParent()->TransferDataToWindow();

	offload_imbalance();

	event.Skip();
	return;
}

void
EntryCtrl::EntryDecimalTextCtrl::offload_imbalance() const
{
	// Autobalance to a DecimalTextCtrl in the same EntryCtrl, which is
	// not this DecimalTextCtrl.
	EntryCtrl* const parent = dynamic_cast<EntryCtrl*>(GetParent());
	assert (parent);
	vector<EntryDecimalTextCtrl*>& amount_boxes = parent->m_amount_boxes;
	assert (amount_boxes.size() > 1);
	if (this == amount_boxes.back())
	{
		assert (amount_boxes[0] != this);
		parent->autobalance(amount_boxes[0]);
	}
	else
	{
		parent->autobalance(amount_boxes.back());
	}
	return;
}


}  // namespace gui
}  // namespace phatbooks
