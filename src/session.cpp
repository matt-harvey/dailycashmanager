/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "session.hpp"
#include "application.hpp"
#include "phatbooks_exceptions.hpp"
#include "gui/app.hpp"
#include <boost/filesystem.hpp>
#include <jewel/assert.hpp>
#include <jewel/log.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <wx/string.h>
#include <memory>
#include <string>

using sqloxx::InvalidFilename;
using std::shared_ptr;
using std::string;
using std::wcscpy;
using std::wstring;

namespace phatbooks
{

using gui::App;

// TODO HIGH PRIORITY
// There are currently the following three classes: Session,
// Application and App. This is left over from when a TUI interface was
// previously provided via TextSession. However we should now be able to
// reduce these to a single class, deriving from wxApp. We should also be
// able to do all our command line parsing using wxWidgets facilities,
// within the App class. We can then remove the dependency on TCLAP.

int Session::s_num_instances = 0;

Session::Session():
	m_existing_application_instance_notified(false)
{
	++s_num_instances;
	if (s_num_instances > s_max_instances)
	{
		--s_num_instances;
		JEWEL_THROW
		(	TooManySessions,
			"Exceeded maximum number of instances of phatbooks::Session."
		);
	}
	JEWEL_ASSERT (s_num_instances <= s_max_instances);
}

Session::~Session()
{
	JEWEL_ASSERT (s_num_instances > 0);
	JEWEL_ASSERT (s_num_instances <= s_max_instances);
	--s_num_instances;
}

void
Session::notify_existing_application_instance()
{
	JEWEL_LOG_TRACE();
	m_existing_application_instance_notified = true;
	return;
}

int
Session::run()
{
	JEWEL_LOG_TRACE();

	shared_ptr<PhatbooksDatabaseConnection> dbc
	(	new PhatbooksDatabaseConnection
	);

	// This is a bit messy, but
	// we do this instead of using the IMPLEMENT_APP macro from
	// wxWidgets, because we don't want IMPLEMENT_APP to provide us
	// with a main function - we already have our own.
	App* app = new App;
	
	if (m_existing_application_instance_notified)
	{
		app->notify_existing_application_instance();
	}

	app->set_database_connection(dbc);
	wxApp::SetInstance(app);
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
Session::run(string const& filepath_str)
{
	JEWEL_LOG_TRACE();

	// TODO LOW PRIORITY We do NOT validate the filepath here. This function
	// is only ever entered if we have an already-existing file which the
	// user opens from the command line. This works OK, but it seems a bit
	// improper somehow.
	
	// TODO LOW PRIORITY There is code duplicated between here and the
	// other form of run
	
	// TODO LOW PRIORITY Make it so that if the file cannot be opened, an
	// informative message is displayed to the user, rather than just crashing
	// with an exception.

	if (filepath_str.empty())
	{
		JEWEL_THROW(InvalidFilename, "Filename is empty string.");
	}

	shared_ptr<PhatbooksDatabaseConnection> dbc
	(	new PhatbooksDatabaseConnection
	);
	boost::filesystem::path const filepath(filepath_str);

	App* app = new App;

	if (m_existing_application_instance_notified)
	{
		app->notify_existing_application_instance();
	}
	else
	{
		dbc->open(filepath);
	}

	app->set_database_connection(dbc);
	wxApp::SetInstance(app);
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

}  // namespace phatbooks
