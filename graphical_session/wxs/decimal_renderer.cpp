#include "decimal_renderer.hpp"
#include "decimal_variant_data.hpp"
#include <wx/dataview.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

DecimalRenderer::DecimalRenderer():
	wxDataViewCustomRenderer
	(	DecimalVariantData::GetTypeStatic(),
		wxDATAVIEW_CELL_EDITABLE,
		wxALIGN_LEFT
	),
	m_decimal(0, 0)
{
}
		
bool
DecimalRenderer::SetValue(wxVariant const& value)
{
	// TODO Implement
}

bool
DecimalRenderer::GetValue(wxVariant& variant) const
{
	// TODO Implement
}

wxSize
DecimalRenderer::GetSize() const
{
	// TODO Implement
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
	// TODO Implement
}

bool
DecimalRenderer::GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value)
{
	// TODO Implement
}
	


}  // namespace phatbooks
}  // namespace gui
