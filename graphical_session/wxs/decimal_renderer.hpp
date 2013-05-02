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
 * Class for rendering account_type::AccountType data in a wxDataViewCtrl
 */
class DecimalRenderer: public wxDataViewCustomRenderer
{
public:

	DecimalRenderer();

	bool SetValue(wxVariant const& value);

	bool GetValue(wxVariant& variant) const;

	wxSize GetSize() const;

	bool HasEditorCtrl() const;

	wxWindow* CreateEditorCtrl
	(	wxWindow* parent,
		wxRect labelRect,
		wxVariant const& value
	);

	bool GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value);

	bool Render(wxRect cell, wxDC* dc, int state);

protected:

private:

	wxLocale const& locale() const;

	jewel::Decimal m_decimal;	

};  // class DecimalRenderer

}  // namespace phatbooks
}  // namespace gui

#endif  // GUARD_decimal_renderer_hpp
