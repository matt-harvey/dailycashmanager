#ifndef GUARD_my_app_hpp
#define GUARD_my_app_hpp

#include "my_frame.hpp"
#include <wx/wx.h>

namespace phatbooks
{
namespace gui
{

class MyApp:
	public wxApp
{
public:
	virtual bool OnInit();

};

// Implements MyApp& wxGetApp()
DECLARE_APP(MyApp)

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_my_app_hpp
