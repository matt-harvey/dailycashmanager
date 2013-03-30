#ifndef GUARD_app_hpp
#define GUARD_app_hpp

#include "frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <wx/intl.h>
#include <wx/snglinst.h>
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
	virtual bool OnInit();
	virtual int OnExit();
	void set_database_connection
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
	);
	wxLocale const& locale() const;

	PhatbooksDatabaseConnection& database_connection();

private:
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
	wxSingleInstanceChecker* m_checker;
	wxLocale m_locale;
};

// Implements App& wxGetApp()
DECLARE_APP(App)

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_app_hpp
