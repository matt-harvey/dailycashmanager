#ifndef GUARD_phatbooks_text_session_hpp
#define GUARD_phatbooks_text_session_hpp

#include "consolixx.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>



namespace phatbooks
{

// FORWARD DECLARATIONS

class PhatbooksDatabaseConnection;

// CLASSES

class PhatbooksTextSession:
	public consolixx::TextSession
{
public:
	PhatbooksTextSession();
	void run();
private:
	boost::shared_ptr<Menu> m_parent_menu;
	boost::shared_ptr<Menu> m_child_menu;
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;

#warning dummy members follow
	void say_hello();
	void print_numbers();
	void quit();



};







}  // namespace phatbooks


#endif  // GUARD_phatbooks_text_session_hpp
