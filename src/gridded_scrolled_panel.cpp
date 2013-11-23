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

#include "gui/gridded_scrolled_panel.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "gui/locale.hpp"
#include "gui/sizing.hpp"
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>


namespace phatbooks
{
namespace gui
{

GriddedScrolledPanel::GriddedScrolledPanel
(	wxWindow* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	bool p_horizontal_scrolling_enabled
):
	wxScrolledWindow
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		p_size,
		(p_horizontal_scrolling_enabled? (wxVSCROLL | wxHSCROLL): wxVSCROLL) |
			wxTAB_TRAVERSAL
	),
	m_current_row(0),
	m_top_sizer(nullptr),
	m_database_connection(p_database_connection)
{
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);
	int const standard_scrolling_increment = 10;
	int const horizontal_scrolling_rate =
		(p_horizontal_scrolling_enabled? standard_scrolling_increment: 0);
	SetScrollRate(horizontal_scrolling_rate, standard_scrolling_increment);
}

GriddedScrolledPanel::~GriddedScrolledPanel()
{
}

wxGridBagSizer&
GriddedScrolledPanel::top_sizer()
{
	JEWEL_ASSERT (m_top_sizer);
	return *m_top_sizer;
}

int
GriddedScrolledPanel::current_row() const
{
	return m_current_row;
}

void
GriddedScrolledPanel::increment_row()
{
	++m_current_row;
	return;
}

void
GriddedScrolledPanel::decrement_row()
{
	--m_current_row;
	return;
}

void
GriddedScrolledPanel::set_row(int p_row)
{
	m_current_row = p_row;
	return;
}

wxStaticText*
GriddedScrolledPanel::display_text
(	wxString const& p_text,
	int p_column,
	int p_alignment_flags
)
{
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		p_text,
		wxDefaultPosition,
		wxDefaultSize,
		p_alignment_flags
	);
	top_sizer().Add
	(	text,
		wxGBPosition(current_row(), p_column),
		wxDefaultSpan,
		p_alignment_flags
	);
	return text;
}

wxStaticText*
GriddedScrolledPanel::display_decimal
(	jewel::Decimal const& p_decimal,
	int p_column,
	bool p_dash_for_zero
)
{
	DecimalFormatFlags flags =
	(	p_dash_for_zero?
		DecimalFormatFlags().set(string_flags::dash_for_zero):
		DecimalFormatFlags().clear(string_flags::dash_for_zero)
	);
	wxStaticText* text = new wxStaticText
	(	this,
		wxID_ANY,
		finformat_wx(p_decimal, locale(), flags),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT
	);
	top_sizer().Add
	(	text,
		wxGBPosition(current_row(), p_column),
		wxDefaultSpan,
		wxALIGN_RIGHT
	);
	return text;
}

PhatbooksDatabaseConnection&
GriddedScrolledPanel::database_connection()
{
	return m_database_connection;
}

PhatbooksDatabaseConnection const&
GriddedScrolledPanel::database_connection() const
{
	return m_database_connection;
}


}  // namespace gui
}  // namespace phatbooks
