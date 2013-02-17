#include "graphical_session.hpp"
#include "string_conv.hpp"
#include "wx/my_app.hpp"
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;
using std::wstring;

namespace phatbooks
{

using string_conv::std8_to_wx;

namespace gui
{

GraphicalSession::GraphicalSession()
{
}

GraphicalSession::~GraphicalSession()
{
}

int
GraphicalSession::do_run(string const& filename)
{
	// This is a bit messy, but
	// we do this instead of using the IMPLEMENT_APP macro from
	// wxWidgets, because we don't want IMPLEMENT_APP to provide us
	// with a main function - we already have our own.
	wxApp* pApp = new MyApp();
	wxApp::SetInstance(pApp);
	int dummy = 0;
	wstring const argv1(filename.begin(), filename.end());

	// WARNING This sucks.
	wchar_t* argv1wc = const_cast<wchar_t*>(argv1.c_str());
	wxEntryStart(dummy, &argv1wc);
	wxTheApp->OnInit();
	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();
	return 0;
}


}  // namespace gui
}  // namespace phatbooks
