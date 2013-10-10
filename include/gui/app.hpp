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


#ifndef GUARD_app_hpp_19666019230925488
#define GUARD_app_hpp_19666019230925488

#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <wx/intl.h>
#include <wx/wx.h>
#include <memory>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class Frame;

// end forward declarations


class App: public wxApp
{
public:
	App();
	App(App const&) = delete;
	App(App&&) = delete;
	App& operator=(App const&) = delete;
	App& operator=(App&&) = delete;
	~App() = default;

	virtual bool OnInit();

	virtual int OnExit();

	void set_database_connection
	(	std::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
	);

	/**
	 * Notify session of existing application instance (which could
	 * be either a console or a graphical session), so that just after
	 * the wxWidgets initialization code has run, it can display an
	 * appropriate message box and abort.
	 */
	void notify_existing_application_instance();

	wxLocale const& locale() const;

	PhatbooksDatabaseConnection& database_connection();

private:
	boost::filesystem::path elicit_existing_filepath();
	std::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
	wxLocale m_locale;
	bool m_existing_application_instance_notified;
};

// Implements App& wxGetApp()
DECLARE_APP(App)

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_app_hpp_19666019230925488
