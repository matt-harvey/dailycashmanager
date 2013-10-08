// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "gui/locale.hpp"
#include "gui/app.hpp"
#include <wx/intl.h>

namespace phatbooks
{
namespace gui
{

wxLocale const&
locale()
{
	App* app = dynamic_cast<App*>(wxTheApp);
	return app->locale();
}

}  // namespace gui
}  // namespace phatbooks
