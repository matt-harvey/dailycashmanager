#include "entry_ctrl.hpp"
#include "account.hpp"
#include "account_ctrl.hpp"
#include "account_reader.hpp"
#include "transaction_type.hpp"
#include <boost/scoped_ptr.hpp>
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <cassert>
#include <vector>

using boost::scoped_ptr;
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
	assert (m_split_buttons.empty());

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
	for ( ; it != end; ++it, ++m_next_row)
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
			wxSize(m_text_ctrl_size.x * 4, m_text_ctrl_size.y),
			wxALIGN_LEFT
		);
		m_top_sizer->Add(comment_ctrl, wxGBPosition(m_next_row, 1), wxGBSpan(1, 2));
		m_comment_boxes.push_back(comment_ctrl);
		wxButton* split_button = new wxButton
		(	this,
			s_split_button_id,
			wxString("Split..."),
			wxDefaultPosition,
			m_text_ctrl_size
		);
		m_top_sizer->Add(split_button, wxGBPosition(m_next_row, 3));
		m_split_buttons.push_back(split_button);
	}
	
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	Layout();
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
		wxSize(m_text_ctrl_size.x * 4, m_text_ctrl_size.y),
		wxALIGN_LEFT
	);
	m_top_sizer->
		Add(comment_ctrl, wxGBPosition(m_next_row, 1), wxGBSpan(1, 2));
	m_comment_boxes.push_back(comment_ctrl);
	wxButton* split_button = new wxButton
	(	this,
		s_split_button_id,
		wxString("Split..."),
		wxDefaultPosition,
		m_text_ctrl_size
	);
	m_top_sizer->Add(split_button, wxGBPosition(m_next_row, 3));
	m_split_buttons.push_back(split_button);

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
