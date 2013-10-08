// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "application.hpp"
#include "gui/app.hpp"
#include "gui/graphical_session.hpp"
#include <boost/filesystem.hpp>
#include <jewel/log.hpp>
#include <wx/string.h>
#include <memory>
#include <string>

using std::shared_ptr;
using std::string;
using std::wcscpy;
using std::wstring;

namespace phatbooks
{


namespace gui
{


GraphicalSession::GraphicalSession():
	m_existing_application_instance_notified(false)
{
}

GraphicalSession::~GraphicalSession()
{
}

void
GraphicalSession::notify_existing_application_instance()
{
	JEWEL_LOG_TRACE();
	m_existing_application_instance_notified = true;
	return;
}

int
GraphicalSession::do_run()
{
	JEWEL_LOG_TRACE();

	shared_ptr<PhatbooksDatabaseConnection> dbc
	(	new PhatbooksDatabaseConnection
	);

	// This is a bit messy, but
	// we do this instead of using the IMPLEMENT_APP macro from
	// wxWidgets, because we don't want IMPLEMENT_APP to provide us
	// with a main function - we already have our own.
	App* pApp = new App;
	
	if (m_existing_application_instance_notified)
	{
		pApp->notify_existing_application_instance();
	}

	pApp->set_database_connection(dbc);
	wxApp::SetInstance(pApp);
	wxString const app_name = Application::application_name();

	// The argv array required by wxEntryStart must be an array
	// of wchar_t*. We produce these as follows.
	wstring const argv0_w(app_name.begin(), app_name.end());

	// We do all this to avoid a const_cast.
	// The extra 1000 is a safeguard against the fact that
	// wxEntryStart (below) may modify the contents of the pointers
	// passed to it. wxWidgets documentation does not say what it might write.
	// We don't want it writing off the end. Yes, this
	// is a grotesque hack.
	size_t buf_0_sz = argv0_w.size() + 1 + 1000;
	wchar_t* buf_0 = new wchar_t[buf_0_sz];
	wcscpy(buf_0, argv0_w.c_str());

	// We now construct the arguments required by wxEntryStart.
	wchar_t* argvs[] = { buf_0, 0 };
	int argca = 0;
	while (argvs[argca] != 0) ++argca;

	// At last...
	wxEntryStart(argca, argvs);
	if (wxTheApp->OnInit())
	{
		wxTheApp->OnRun();
	}
	else
	{
		// User has cancelled rather than opening a file
		// Nothing to do.
	}
	wxTheApp->OnExit();
	wxEntryCleanup();

	delete[] buf_0;
	buf_0 = nullptr;

	JEWEL_LOG_TRACE();

	return 0;
}


int
GraphicalSession::do_run(string const& filepath_str)
{
	JEWEL_LOG_TRACE();

	// TODO Validate the filepath here first - similar to what
	// we did in TextSession::do_run(...) (see branches/tui_branch in
	// repository).
	
	// TODO There is code duplicated between here and the
	// other form of do_run

	// TODO Is this safe? We need to catch and handle if we can't open
	shared_ptr<PhatbooksDatabaseConnection> dbc
	(	new PhatbooksDatabaseConnection
	);
	boost::filesystem::path const filepath(filepath_str);

	App* pApp = new App;

	if (m_existing_application_instance_notified)
	{
		pApp->notify_existing_application_instance();
	}
	else
	{
		dbc->open(filepath);
	}

	pApp->set_database_connection(dbc);
	wxApp::SetInstance(pApp);
	wxString const app_name = Application::application_name();	

	// The argv array required by wxEntryStart must be an
	// array of wchar_t*. We produce these as follows.
	wstring const argv0_w(app_name.begin(), app_name.end());
	wstring const argv1_w(filepath_str.begin(), filepath_str.end());

	// We do all this to avoid a const_cast.
	// The extra 1000 is a safeguard against the fact that
	// wxEntryStart (below) may modify the contents of the pointers
	// passed to it. wxWidgets documentation does not say what it might write.
	// We don't want it writing off the end. Yes, this
	// is a grotesque hack.
	size_t buf_0_sz = argv0_w.size() + 1 + 1000;
	size_t buf_1_sz = argv1_w.size() + 1 + 1000;
	wchar_t* buf_0 = new wchar_t[buf_0_sz];
	wchar_t* buf_1 = new wchar_t[buf_1_sz];
	wcscpy(buf_0, argv0_w.c_str());
	wcscpy(buf_1, argv1_w.c_str());

	// We now construct the arguments required by wxEntryStart.
	wchar_t* argvs[] = { buf_0, buf_1, 0 };
	int argca = 0;
	while (argvs[argca] != 0) ++argca;

	// At last...
	wxEntryStart(argca, argvs);
	wxTheApp->OnInit();
	wxTheApp->OnRun();
	wxTheApp->OnExit();
	wxEntryCleanup();

	delete[] buf_0;
	buf_0 = nullptr;
	delete[] buf_1;
	buf_1 = nullptr;
	
	JEWEL_LOG_TRACE();

	return 0;
}


}  // namespace gui
}  // namespace phatbooks
