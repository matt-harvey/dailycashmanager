#ifndef GUARD_decimal_renderer_hpp
#define GUARD_decimal_renderer_hpp

#include <jewel/decimal.hpp>
#include <wx/dataview.h>
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/window.h>

namespace phatbooks
{
namespace gui
{

/**
 * Class for rendering jewel::Decimal data in a wxDataViewCtrl
 */
class DecimalRenderer: public wxDataViewCustomRenderer
{
public:

	DecimalRenderer(jewel::Decimal::places_type p_precision);

	bool SetValue(wxVariant const& value);

	bool GetValue(wxVariant& value) const;

	wxSize GetSize() const;

	bool HasEditorCtrl() const;

	wxWindow* CreateEditorCtrl
	(	wxWindow* parent,
		wxRect labelRect,
		wxVariant const& value
	);

	bool GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value);

	bool Render(wxRect rect, wxDC* dc, int state);

protected:

private:

	wxLocale const& locale() const;

	jewel::Decimal m_decimal;	

};  // class DecimalRenderer

}  // namespace phatbooks
}  // namespace gui

#endif  // GUARD_decimal_renderer_hpp
