// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_app_hpp
#define GUARD_app_hpp

#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <wx/intl.h>
#include <wx/wx.h>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class Frame;

// end forward declarations


class App:
	public wxApp
{
public:
	App();

	virtual bool OnInit();

	virtual int OnExit();

	void set_database_connection
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
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
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
	wxLocale m_locale;
	bool m_existing_application_instance_notified;
};

// Implements App& wxGetApp()
DECLARE_APP(App)

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_app_hpp
