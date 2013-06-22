#include "entry_ctrl.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "decimal_text_ctrl.hpp"
#include "entry.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include "transaction_type.hpp"
#include <boost/scoped_ptr.hpp>
#include <jewel/decimal.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <cassert>
#include <vector>

using boost::scoped_ptr;
using jewel::Decimal;
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
(	wxWindow* p_parent,
	vector<Account> const& p_accounts,
	PhatbooksDatabaseConnection& p_database_connection,
	transaction_type::TransactionType p_transaction_type,
	wxSize const& p_text_ctrl_size,
	bool p_is_source,
	Decimal const& p_primary_amount
):
	wxPanel(p_parent),
	m_database_connection(p_database_connection),
	m_is_source(p_is_source),
	m_primary_amount(p_primary_amount),
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

	m_top_sizer = new wxGridBagSizer();
	SetSizer(m_top_sizer);

	// Row 0

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
	if (p_accounts.size() > 1)
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

	// Subsequent rows

	++m_next_row;

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

	vector<Account>::const_iterator it = p_accounts.begin();
	vector<Account>::const_iterator const end = p_accounts.end();

	// TODO Factor this out. (Re-used add_row().)
	for (size_t entry_num = 0; it != end; ++it, ++m_next_row, ++entry_num)
	{
		AccountCtrl* account_name_box = new AccountCtrl
		(	this,
			wxID_ANY,
			*it,
			m_text_ctrl_size,
			m_account_reader->begin(),
			m_account_reader->end(),
			m_database_connection
		);
		m_top_sizer->Add(account_name_box, wxGBPosition(m_next_row, 0));
		m_account_name_boxes.push_back(account_name_box);
		wxTextCtrl* comment_ctrl = new wxTextCtrl
		(	this,
			wxID_ANY,
			wxEmptyString,
			wxDefaultPosition,
			wxSize(m_text_ctrl_size.x * 2, m_text_ctrl_size.y),
			wxALIGN_LEFT
		);
		m_top_sizer->
			Add(comment_ctrl, wxGBPosition(m_next_row, 1), wxGBSpan(1, 2));
		m_comment_boxes.push_back(comment_ctrl);

		// If there is only one Account then there is only one "Entry line",
		// and that Entry line will also house the "Split" button. There will
		// then be no need to have an amount_ctrl for just the one
		// Entry. If there are multiple Entries then we must have already
		// placed the Split button at row 0, and in that case we also need
		// an amount_ctrl for each Entry.
		if (p_accounts.size() == 1)
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
			Decimal::places_type const precision = m_primary_amount.places();
			DecimalTextCtrl* amount_ctrl = new DecimalTextCtrl
			(	this,
				wxID_ANY,
				m_text_ctrl_size,
				precision,
				false
			);
			if (entry_num == 0)
			{
				amount_ctrl->set_amount(m_primary_amount);
			}
			m_top_sizer->Add(amount_ctrl, wxGBPosition(m_next_row, 3));
			m_amount_boxes.push_back(amount_ctrl);
		}
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
EntryCtrl::refresh_for_transaction_type
(	transaction_type::TransactionType p_transaction_type
)
{
	JEWEL_DEBUG_LOG << "Entered EntryCtrl::refresh_for_transaction_type." << endl;
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

void
EntryCtrl::set_primary_amount(Decimal const& p_primary_amount)
{
	m_primary_amount = p_primary_amount;
	typedef vector<DecimalTextCtrl*>::size_type Size;
	Size const sz = m_amount_boxes.size();
	if (sz > 0)
	{
		assert (m_amount_boxes[0]);
		m_amount_boxes[0]->set_amount(m_primary_amount);
		for (Size i = 1; i != sz; ++i)
		{
			assert (m_amount_boxes[i]);
			m_amount_boxes[i]->
				set_amount(Decimal(0, m_primary_amount.places()));
		}
	}
	return;
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

		Decimal amount = m_primary_amount;
		JEWEL_DEBUG_LOG << "m_primary_amount: " << m_primary_amount << endl;
		if (m_amount_boxes.size() != 0)
		{
			JEWEL_DEBUG_LOG_LOCATION;
			assert (m_amount_boxes.size() == sz);
			assert (sz > 1);
			assert (m_amount_boxes[i]);
			JEWEL_DEBUG_LOG << "m_amount_boxes[i]->amount(): "
			                << m_amount_boxes[i]->amount()
							<< endl;
			JEWEL_DEBUG_LOG << "m_amount_boxes[i]->GetValue(): "
			                << m_amount_boxes[i]->GetValue()
							<< endl;
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
		JEWEL_DEBUG_LOG << "amount: " << amount << endl;
		entry.set_amount(amount);
		JEWEL_DEBUG_LOG << "entry.amount(): " << entry.amount() << endl;

		entry.set_whether_reconciled(false);

		ret.push_back(entry);
		assert (!entry.has_id());
	}
	assert (ret.size() == sz);
	return ret;
}

void
EntryCtrl::on_split_button_click(wxCommandEvent& event)
{
	(void)event;  // Silence compiler warning re. unused parameter.
	add_row();
	return;
}

void
EntryCtrl::add_row()
{
	assert (m_account_name_boxes.size() >= 1);
	AccountCtrl* account_name_box = new AccountCtrl
	(	this,
		wxID_ANY,
		m_account_name_boxes.at(m_account_name_boxes.size() - 1)->account(),
		m_text_ctrl_size,
		m_account_reader->begin(),
		m_account_reader->end(),
		m_database_connection
	);
	m_top_sizer->Add(account_name_box, wxGBPosition(m_next_row, 0));
	m_account_name_boxes.push_back(account_name_box);
	wxTextCtrl* comment_ctrl = new wxTextCtrl
	(	this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(m_text_ctrl_size.x * 2, m_text_ctrl_size.y),
		wxALIGN_LEFT
	);
	m_top_sizer->
		Add(comment_ctrl, wxGBPosition(m_next_row, 1), wxGBSpan(1, 2));
	m_comment_boxes.push_back(comment_ctrl);

	DecimalTextCtrl* amount_ctrl = new DecimalTextCtrl
	(	this,
		wxID_ANY,
		m_text_ctrl_size,
		m_primary_amount.places(),
		false
	);
	m_top_sizer->Add(amount_ctrl, wxGBPosition(m_next_row, 3));

	if (m_amount_boxes.size() == 0)
	{
		// Then the initial Entry line now needs a DecimalTextCtrl too, and
		// we need to reposition m_split_button to make way for it.
		m_top_sizer->Detach(m_split_button);
		assert (m_next_row >= 2);
		m_top_sizer->Add(m_split_button, wxGBPosition(m_next_row - 2, 3));
		assert (!m_account_name_boxes.empty());
		m_split_button->MoveBeforeInTabOrder(m_account_name_boxes[0]);
		DecimalTextCtrl* prev_amount_ctrl = new DecimalTextCtrl
		(	this,
			wxID_ANY,
			m_text_ctrl_size,
			m_primary_amount.places(),
			false
		);
		prev_amount_ctrl->set_amount(m_primary_amount);
		m_top_sizer->Add(prev_amount_ctrl, wxGBPosition(m_next_row - 1, 3));
		prev_amount_ctrl->MoveBeforeInTabOrder(account_name_box);
		assert (m_amount_boxes.empty());
		m_amount_boxes.push_back(prev_amount_ctrl);
	}

	m_amount_boxes.push_back(amount_ctrl);

	++m_next_row;

	Layout();  // Must call this.
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	// GetParent()->Layout(); // Do not call this.
	// GetParent()->GetSizer()->RecalcSizes();  // Do not call this.
	GetParent()->Fit();

	return;
}

wxString
EntryCtrl::side_description() const
{
	if (m_is_source)
	{
		return " Source:";
	}
	assert (!m_is_source);
	return " Destination:";
}

}  // namespace gui
}  // namespace phatbooks
