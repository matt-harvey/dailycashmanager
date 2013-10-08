// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "gui/window_utilities.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/optional.hpp>
#include <wx/string.h>
#include <wx/window.h>

using boost::optional;
using jewel::value;

namespace phatbooks
{
namespace gui
{

void
toggle_enabled
(	wxWindow* p_window,
	bool p_enable,
	optional<wxString> const& p_tooltip_if_disabled
)
{
	JEWEL_ASSERT (p_window);
	if (p_enable)
	{
		p_window->Enable();
		p_window->SetToolTip(0);
	}
	else
	{
		p_window->Disable();
		if (p_tooltip_if_disabled)
		{
			p_window->SetToolTip(value(p_tooltip_if_disabled));
		}
	}
	return;
}

}  // namespace gui
}  // namespace phatbooks
