#include "graphical_session.hpp"
#include "wx/my_app.hpp"
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;

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
	int dummy = 0;
	char* argv_proxy[] = { const_cast<char*>(filename.c_str()) };
	wxEntryStart(dummy, argv_proxy);
	wxTheApp->OnInit();
	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();
	return 0;
}


}  // namespace gui
}  // namespace phatbooks
