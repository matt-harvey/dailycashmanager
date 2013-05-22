// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "decimal_renderer.hpp"
#include "decimal_variant_data.hpp"
#include "app.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include <jewel/debug_log.hpp>
#include <jewel/decimal.hpp>
#include <wx/app.h>
#include <wx/dataview.h>
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/window.h>

using jewel::Decimal;
using jewel::round;

namespace phatbooks
{
namespace gui
{

DecimalRenderer::DecimalRenderer(Decimal::places_type p_precision):
	wxDataViewCustomRenderer
	(	"string",
		wxDATAVIEW_CELL_EDITABLE,
		wxALIGN_RIGHT
	),
	m_decimal(0, p_precision),
	m_precision(p_precision)
{
}
		
bool
DecimalRenderer::SetValue(wxVariant const& value)
{
	try
	{
		Decimal const d = wx_to_decimal(value.GetString(), locale());
		m_decimal = round(d, m_precision);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool
DecimalRenderer::GetValue(wxVariant& value) const
{
	try
	{
		value = finformat_wx(m_decimal, locale());
		return true;
	}
	catch (...)
	{
		return false;
	}
}

wxSize
DecimalRenderer::GetSize() const
{
	return wxSize(60, 20);  // TODO Make this more robust/adaptive/whatever
}

bool
DecimalRenderer::HasEditorCtrl() const
{
	return true;
}

wxWindow*
DecimalRenderer::CreateEditorCtrl
(	wxWindow* parent,
	wxRect labelRect,
	wxVariant const& value
)
{
	return new wxTextCtrl
	(	parent,
		wxID_ANY,
		value.GetString(),
		labelRect.GetTopLeft(),
		labelRect.GetSize(),
		wxALIGN_RIGHT,
		wxDefaultValidator  // TODO HIGH PRIORITY Use a proper validator
	);
}

bool
DecimalRenderer::GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value)
{
	wxTextCtrl* const text_ctrl = dynamic_cast<wxTextCtrl*>(editor);
	if (!text_ctrl)
	{
		return false;
	}
	try
	{
		value = text_ctrl->GetValue();
		return true;
	}
	catch (...)
	{
		return false;
	}
}
	
bool
DecimalRenderer::Render(wxRect rect, wxDC* dc, int state)
{
	try
	{
		RenderText
		(	finformat_wx(m_decimal, locale()),
			0,  // no offset
			rect,
			dc,
			state
		);
		return true;
	}
	catch (...)
	{
		return false;
	}
}



}  // namespace phatbooks
}  // namespace gui
