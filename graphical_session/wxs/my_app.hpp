#ifndef GUARD_my_app_hpp
#define GUARD_my_app_hpp

#include "my_frame.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <wx/wx.h>

namespace phatbooks
{

class PhatbooksDatabaseConnection;


namespace gui
{

class MyApp:
	public wxApp
{
public:
	virtual bool OnInit();

	void set_database_connection
	(	boost::shared_ptr<PhatbooksDatabaseConnection> p_database_connection
	);
private:
	PhatbooksDatabaseConnection& database_connection()
	{
		return *m_database_connection;
	}
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;

};

// Implements MyApp& wxGetApp()
DECLARE_APP(MyApp)

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_my_app_hpp
