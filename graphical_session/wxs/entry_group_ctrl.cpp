// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "entry_group_ctrl.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "decimal_text_ctrl.hpp"
#include "entry.hpp"
#include "finformat.hpp"
#include "journal.hpp"
#include "locale.hpp"
#include "string_flags.hpp"
#include "transaction_side.hpp"
#include "transaction_type.hpp"
#include "transaction_ctrl.hpp"
#include "sizing.hpp"
#include "window_utilities.hpp"
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <jewel/array_utilities.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <cassert>
#include <set>
#include <vector>

using boost::optional;
using boost::scoped_ptr;
using jewel::Decimal;
using jewel::num_elements;
using jewel::value;
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

BEGIN_EVENT_TABLE(EntryGroupCtrl, wxPanel)
	EVT_BUTTON
	(	s_unsplit_button_id,
		EntryGroupCtrl::on_unsplit_button_click
	)
	EVT_BUTTON
	(	s_split_button_id,
		EntryGroupCtrl::on_split_button_click
	)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(EntryGroupCtrl::EntryDecimalTextCtrl, DecimalTextCtrl)
	EVT_LEFT_DCLICK
	(	EntryGroupCtrl::EntryDecimalTextCtrl::on_left_double_click
	)
END_EVENT_TABLE()

EntryGroupCtrl::EntryGroupCtrl
(	TransactionCtrl* p_parent,
	wxSize const& p_text_ctrl_size,
	Journal const& p_journal,
	transaction_side::TransactionSide p_transaction_side,
	PhatbooksDatabaseConnection& p_database_connection
):
	wxPanel(p_parent),
	m_database_connection(p_database_connection),
	m_transaction_side(p_transaction_side),
	m_transaction_type(p_journal.transaction_type()),
	m_available_account_types(0),
	m_text_ctrl_size(p_text_ctrl_size),
	m_top_sizer(0),
	m_side_descriptor(0),
	m_unsplit_button(0),
	m_split_button(0),
	m_current_row(0)
{
	assert (m_entry_rows.empty());
	assert_transaction_type_validity(m_transaction_type);
	
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);

	// TODO There should really be a function somewhere in the business
	// layer which gives us a vector of Entries for a given Journal
	// and TransactionSide.
	vector<Entry> entries;
	vector<Entry> const& all_entries = p_journal.entries();
	vector<Entry>::const_iterator it = all_entries.begin();
	vector<Entry>::const_iterator const end = all_entries.end();
	for ( ; it != end; ++it)
	{
		if (it->transaction_side() == m_transaction_side)
		{
			entries.push_back(*it);
		}
	}
	bool const multiple_entries = (entries.size() > 1);
	configure_top_row(multiple_entries);
	configure_available_account_types();	
	optional<Decimal> maybe_previous_row_amount;
	for (vector<Entry>::size_type i = 0; i != entries.size(); ++i)
	{
		Entry entry = entries[i];
		push_row(entry, maybe_previous_row_amount, multiple_entries);
		if (i == 0) maybe_previous_row_amount = entry.amount();
	}
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	// Fit();
	// Layout();
	GetParent()->Fit();
}

EntryGroupCtrl::~EntryGroupCtrl()
{
	delete m_available_account_types;
	m_available_account_types = 0;
}

void
EntryGroupCtrl::configure_available_account_types()
{
	// TODO Make this exception safe.
	if (m_available_account_types)
	{
		delete m_available_account_types;
		m_available_account_types = 0;
	}
	assert (!m_available_account_types);
	if (is_source())
	{
		m_available_account_types = new vector<account_type::AccountType>
		(	source_account_types(m_transaction_type)
		);
	}
	else
	{
		m_available_account_types = new vector<account_type::AccountType>
		(	destination_account_types(m_transaction_type)
		);
	}
	return;
}

void
EntryGroupCtrl::configure_top_row(bool p_include_split_button)
{
	assert (m_current_row == 0);
	m_side_descriptor = new wxStaticText(this, wxID_ANY, side_description());
	m_top_sizer->Add(m_side_descriptor, wxGBPosition(m_current_row, 0));

	wxStaticText* const comment_label = new wxStaticText
	(	this,
		wxID_ANY,
		wxString(" Memo:"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_LEFT
	);
	m_top_sizer->Add(comment_label, wxGBPosition(m_current_row, 1));

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
		m_top_sizer->Add(m_split_button, wxGBPosition(m_current_row, 3));
	}
	++m_current_row;
	assert (m_current_row == 1);
	return;
}

void
EntryGroupCtrl::refresh_for_transaction_type
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
	configure_available_account_types();
	for
	(	vector<EntryRow>::size_type i = 0;
		i != m_entry_rows.size();
		++i
	)
	{
		m_entry_rows[i].account_ctrl->reset(*m_available_account_types);
	}
	m_side_descriptor->SetLabel(side_description());
	Layout();
	return;
}

Decimal
EntryGroupCtrl::primary_amount() const
{
	TransactionCtrl const* const parent =
		dynamic_cast<TransactionCtrl const*>(GetParent());
	assert (parent);
	return parent->primary_amount();
}

vector<Entry>
EntryGroupCtrl::make_entries() const
{
	typedef std::vector<Entry>::size_type Size;
	Size const sz = m_entry_rows.size();

	vector<Entry> ret;
	for (Size i = 0; i != sz; ++i)
	{
		EntryRow const& entry_row = m_entry_rows[i];
		Entry entry = entry_row.entry;
		entry.set_account(entry_row.account_ctrl->account());
		entry.set_comment(entry_row.comment_ctrl->GetValue());

		Decimal amount = primary_amount();
		if (entry_row.amount_ctrl)
		{
			amount = entry_row.amount_ctrl->amount();
		}
		if (!transaction_type_is_actual(m_transaction_type))
		{
			amount = -amount;
		}
		if (is_source())
		{
			amount = -amount;
		}
		entry.set_amount(amount);
	
		// Leave reconciliation status as as, as user cannot change it
		// via TransactionCtrl / EntryGroupCtrl.

		assert (entry.transaction_side() == m_transaction_side);

		ret.push_back(entry);
	}
	assert (ret.size() == sz);
	return ret;
}

bool
EntryGroupCtrl::is_all_zero() const
{
	assert (!m_entry_rows.empty());
	Decimal const zero(0, 0);
	if (m_entry_rows.size() == 1)
	{
		return primary_amount() == zero;
	}
	for
	(	vector<EntryRow>::size_type i = 0;
		i != m_entry_rows.size();
		++i
	)
	{
		assert (m_entry_rows[i].amount_ctrl);
		if (m_entry_rows[i].amount_ctrl->amount() != zero)
		{
			return false;
		}
	}
	return true;
}

Decimal
EntryGroupCtrl::total_amount() const
{
	assert (!m_entry_rows.empty());
	if (!m_entry_rows[0].amount_ctrl)
	{
		return primary_amount();
	}
	vector<EntryRow>::size_type const sz = m_entry_rows.size();
	assert (sz >= 2);
	Decimal ret(0, 0);
	for (vector<EntryRow>::size_type i = 0; i != sz; ++i)
	{
		assert (m_entry_rows[i].amount_ctrl);
		ret += m_entry_rows[i].amount_ctrl->amount();
	}
	return ret;
}

void
EntryGroupCtrl::on_split_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	Account const account = m_entry_rows.back().account_ctrl->account();
	Entry entry(m_database_connection);
	entry.set_account(account);
	entry.set_whether_reconciled(false);
	entry.set_comment(wxString());
	entry.set_amount(Decimal(0, account.commodity().precision()));
	entry.set_transaction_side(m_transaction_side);
	push_row(entry, optional<Decimal>(), true);
	assert (!m_entry_rows.empty());
	autobalance(m_entry_rows.back().amount_ctrl);
	return;
}

bool
EntryGroupCtrl::is_source() const
{
	return m_transaction_side == transaction_side::source;
}

void
EntryGroupCtrl::on_unsplit_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	pop_row();
	return;
}

void
EntryGroupCtrl::pop_row()
{
	if (m_entry_rows.size() == 1)
	{
		return;
	}
	assert (m_entry_rows.size() > 1);
	EntryRow& doomed_entry_row = m_entry_rows.back();
	wxWindow* const doomed_windows[] =
	{	doomed_entry_row.account_ctrl,
		doomed_entry_row.comment_ctrl,
		doomed_entry_row.amount_ctrl
	};
	for (size_t i = 0; i != num_elements(doomed_windows); ++i)
	{
		wxWindow* doomed_window = doomed_windows[i];
		if (doomed_window)
		{
			m_top_sizer->Detach(doomed_window);
			doomed_window->Destroy();
		}
	}
	m_entry_rows.pop_back();
	if (m_entry_rows.size() == 1)
	{
		// Rearrange things to destroy m_unsplit_button, and move
		// m_split_button to its "inline" position, in place of the remaining
		// EntryDecimalAmountCtrl.
		m_top_sizer->Detach(m_entry_rows.back().amount_ctrl);
		m_entry_rows.back().amount_ctrl->Destroy();
		m_entry_rows.back().amount_ctrl = 0;
		m_top_sizer->Detach(m_unsplit_button);
		m_unsplit_button->Destroy();
		m_unsplit_button = 0;
		m_top_sizer->Detach(m_split_button);
		m_top_sizer->Add(m_split_button, wxGBPosition(1, 3));
		m_split_button->
			MoveAfterInTabOrder(m_entry_rows.back().comment_ctrl);
	}
	--m_current_row;
	adjust_layout_for_new_number_of_rows();
	return;
}

void
EntryGroupCtrl::push_row
(	Entry const& p_entry,
	optional<Decimal> const& p_previous_row_amount,
	bool p_multiple_entries
)
{
	// EntryRow
	EntryRow entry_row(p_entry);

	// Account name widget
	entry_row.account_ctrl = new AccountCtrl
	(	this,
		wxID_ANY,
		m_text_ctrl_size,
		*m_available_account_types,
		m_database_connection
	);
	entry_row.account_ctrl->set_account(p_entry.account());
	m_top_sizer->
		Add(entry_row.account_ctrl, wxGBPosition(m_current_row, 0));

	// Comment widget
	entry_row.comment_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		p_entry.comment(),
		wxDefaultPosition,
		wxSize(m_text_ctrl_size.x * 2 + standard_gap(), m_text_ctrl_size.y),
		wxALIGN_LEFT
	);
	m_top_sizer->Add
	(	entry_row.comment_ctrl,
		wxGBPosition(m_current_row, 1),
		wxGBSpan(1, 2)
	);

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
		m_top_sizer->Add(m_split_button, wxGBPosition(m_current_row, 3));
		assert (entry_row.amount_ctrl == 0);
	}
	else
	{
		assert (p_multiple_entries);
		entry_row.amount_ctrl = new EntryDecimalTextCtrl
		(	this,
			m_text_ctrl_size
		);
		if (m_current_row == 2)
		{
			// Then this is either the 0 or the 1 entry row. The 0 one
			// doesn't have an amount box...
			// The 0 Entry line now needs a EntryDecimalTextCtrl too, and
			// we need to reposition m_split_button to make way for it.
			m_unsplit_button = new wxButton
			(	this,
				s_unsplit_button_id,
				wxString("Unsplit"),
				wxDefaultPosition,
				m_text_ctrl_size
			);
			m_top_sizer->Add
			(	m_unsplit_button,
				wxGBPosition(0, 2),
				wxDefaultSpan,
				wxALIGN_RIGHT
			);
			assert (!m_entry_rows.empty());
			if (!m_entry_rows.back().amount_ctrl)
			{
				m_top_sizer->Detach(m_split_button);
				m_top_sizer->Add(m_split_button, wxGBPosition(0, 3));
				m_split_button->
					MoveBeforeInTabOrder(m_entry_rows[0].account_ctrl);
				EntryDecimalTextCtrl* prev_amount_ctrl =
					new EntryDecimalTextCtrl
					(	this,
						m_text_ctrl_size
					);
				if (p_previous_row_amount)
				{
					prev_amount_ctrl->
						set_amount(value(p_previous_row_amount));
				}
				else
				{
					prev_amount_ctrl->set_amount(primary_amount());
				}
				m_top_sizer->
					Add(prev_amount_ctrl, wxGBPosition(1, 3));
				prev_amount_ctrl->
					MoveBeforeInTabOrder(entry_row.account_ctrl);
				m_entry_rows.back().amount_ctrl = prev_amount_ctrl;
			}
			m_unsplit_button->MoveBeforeInTabOrder(m_split_button);
		}
		Decimal amount = (is_source()? -p_entry.amount(): p_entry.amount());
		if (m_transaction_type == transaction_type::envelope_transaction)
		{
			amount = -amount;
		}
		entry_row.amount_ctrl->set_amount(amount);
		m_top_sizer->
			Add(entry_row.amount_ctrl, wxGBPosition(m_current_row, 3));
	}

	m_entry_rows.push_back(entry_row);

	++m_current_row;

	adjust_layout_for_new_number_of_rows();

	return;
}

void
EntryGroupCtrl::adjust_layout_for_new_number_of_rows()
{
	Layout();  // Must call this.
	
	TransactionCtrl* parent = dynamic_cast<TransactionCtrl*>(GetParent());
	assert (parent);

	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	// GetParent()->Layout(); // Do not call this.
	// GetParent()->GetSizer()->RecalcSizes();  // Do not call this.
	parent->FitInside();
	// parent->Fit();  // Do not call this.
	
	reflect_reconciliation_statuses();

	return;
}

wxString
EntryGroupCtrl::side_description() const
{
	// WARNING This is pretty inefficient. But it probably doesn't
	// matter.
	wxString ret(" ");
	set<account_super_type::AccountSuperType> super_types;
	switch (m_transaction_type)
	{
	case transaction_type::expenditure_transaction:  // fall through
	case transaction_type::revenue_transaction:
		if (is_source()) source_super_types(m_transaction_type, super_types);
		else destination_super_types(m_transaction_type, super_types);
		assert(super_types.size() == 1);
		ret += account_concept_name
		(	*(super_types.begin()),
			AccountPhraseFlags().set(string_flags::capitalize)
		);
		break;
	case transaction_type::balance_sheet_transaction: // fall through
	case transaction_type::envelope_transaction:  // fall through
		ret += (is_source()? wxString("Source"): wxString("Destination"));
		break;
	case transaction_type::generic_transaction:
		ret += (is_source()? wxString("CR"): wxString("DR"));
		break;
	default:
		assert (false);
	}
	ret += ":";
	return ret;
}

bool
EntryGroupCtrl::reflect_reconciliation_statuses()
{
	bool ret = false;
	vector<int>::size_type i = 0;
	vector<int>::size_type const sz = m_entry_rows.size();
	for ( ; i != sz; ++i)
	{
		EntryRow const& entry_row = m_entry_rows[i];
		vector<wxWindow*> window_vec;
		window_vec.push_back(entry_row.account_ctrl);
		window_vec.push_back(entry_row.comment_ctrl);
		if (entry_row.amount_ctrl)
		{
			window_vec.push_back(entry_row.amount_ctrl);
		}
		bool const reconciled = entry_row.entry.is_reconciled();
		if (reconciled)
		{
			ret = true;
		}
		vector<wxWindow*>::iterator it = window_vec.begin();
		vector<wxWindow*>::iterator const end = window_vec.end();
		for ( ; it != end; ++it)
		{
			assert (*it);
			toggle_enabled
			(	*it,
				!reconciled,
				wxString
				(	"This transaction line has been marked as reconciled, and"
					" so cannot be edited. To review reconciliations, go to "
					"the reconciliations tab."
				)
			);
		}
	}
	// If the last Entry row is reconciled, then we must disable the
	// "Unsplit" button. If there's only one row, then we should disable
	// the "Split" button (and there will be on unsplit button).
	bool const final_row_reconciled =
		m_entry_rows.back().entry.is_reconciled();
	if (m_unsplit_button)
	{
		assert (num_rows() >= 2);
		toggle_enabled(m_unsplit_button, !final_row_reconciled);
	}
	else
	{
		assert (num_rows() == 1);
		assert (m_split_button);
		toggle_enabled(m_split_button, !final_row_reconciled);
	}
	return ret;
}

void
EntryGroupCtrl::autobalance(EntryDecimalTextCtrl* p_target)
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

size_t
EntryGroupCtrl::num_rows() const
{
	return m_entry_rows.size();
}

EntryGroupCtrl::EntryDecimalTextCtrl::EntryDecimalTextCtrl
(	EntryGroupCtrl* p_parent,
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
	// TODO We should have an application-level setting where user can
	// disable tooltips.
	SetToolTip
	(	wxString
		(	"Double-click to balance transaction automatically to this line."
		)
	);
}

EntryGroupCtrl::EntryDecimalTextCtrl::~EntryDecimalTextCtrl()
{
}

void
EntryGroupCtrl::EntryDecimalTextCtrl::on_left_double_click(wxMouseEvent& event)
{
	(void)event;  // silence compiler re. unused parameter
	EntryGroupCtrl* const parent = dynamic_cast<EntryGroupCtrl*>(GetParent());
	assert (parent);
	parent->autobalance(this);
	return;
}

EntryGroupCtrl::EntryRow::EntryRow(Entry const& p_entry):
	account_ctrl(0),
	comment_ctrl(0),
	amount_ctrl(0),
	entry(p_entry)
{
}


}  // namespace gui
}  // namespace phatbooks
