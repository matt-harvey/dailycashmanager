#include "locale.hpp"
#include "app.hpp"
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
