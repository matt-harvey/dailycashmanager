#include "phatbooks_text_session.hpp"

#include "consolixx.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

using consolixx::get_user_input;
using consolixx::get_constrained_user_input;
using consolixx::TextSession;
using sqloxx::SQLiteException;
using boost::bind;
using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;

namespace phatbooks
{


PhatbooksTextSession::PhatbooksTextSession():
	m_main_menu(new Menu),
	m_database_connection(new PhatbooksDatabaseConnection)
{
	// Set up all the Menu objects.

	shared_ptr<MenuItem> quit_item
	(	new MenuItem
		(	"Quit",
			bind(&PhatbooksTextSession::wrap_up, this),
			false,
			"x"
		)
	);
	m_main_menu->add_item(quit_item);
}


bool has_three_letters(string const& s)
{
	return s.size() == 3;
}

void PhatbooksTextSession::run()
{
	bool successful = false;
	while (!successful)
	{
		cout << "Enter file to open (will be created if doesn't already "
		     << "exist): ";
		string filename = get_user_input();
		try
		{
			m_database_connection->open(filename.c_str());
			successful = true;
		}
		catch (SQLiteException&)
		{
			cout << "File named \"" << filename << "\" could not be opened."
			     << " Please try again."
				 << endl;
			successful = false;
		}
	}
	m_database_connection->setup();
	m_main_menu->present_to_user();
	wrap_up();
	return;
}

void PhatbooksTextSession::run(string const& filename)
{
	m_database_connection->open(filename.c_str());
	m_database_connection->setup();
	m_main_menu->present_to_user();	
	wrap_up();
	return;
}
	
void PhatbooksTextSession::wrap_up()
{
	return;
}


}  // namespace phatbooks
