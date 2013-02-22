#include "graphical_session.hpp"
#include "application.hpp"
#include "b_string.hpp"
#include "wxs/my_app.hpp"
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;
using std::wstring;

namespace phatbooks
{


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
	BString const app_name = Application::application_name();	

	wstring const argv0_w(app_name.begin(), app_name.end());
	wstring const argv1_w(filename.begin(), filename.end());
	int argca = 2;

	// WARNING This sucks.
	wchar_t* argv0_wct = const_cast<wchar_t*>(app_name_w.c_str());
	wchar_t* argv1_wct = const_cast<wchar_t*>(argv1.c_str());
	wchar_t* argvs[2] = { argv0_wct, argv1_wct };
	wxEntryStart(argca, argvs);
	wxTheApp->OnInit();
	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();
	return 0;
}


}  // namespace gui
}  // namespace phatbooks
