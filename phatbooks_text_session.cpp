#include "phatbooks_text_session.hpp"

#include "consolixx.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx_exceptions.hpp"
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
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

PhatbooksTextSession::~PhatbooksTextSession()
{
	wrap_up();
}

bool has_three_letters(string const& s)
{
	return s.size() == 3;
}

namespace
{
	bool is_yes_no(string const& s)
	{
		return (s == "y" || s == "n");
	}
}

int PhatbooksTextSession::run(string const& filename)
{
	boost::filesystem::file_status s =
		boost::filesystem::status(boost::filesystem::path(filename));
	if (!boost::filesystem::exists(s))
	{
		cout << "File does not exist. "
		     << "Create file \"" << filename << "\"? (y/n): ";
		string const response = get_constrained_user_input
		(	is_yes_no,
			"Try again, entering 'y' to create file, or 'n' to abort: ",
			false
		);
		if (response != "y")
		{
			assert (response == "n");
			cout << "Exiting program." << endl;
			return 1;
		}
		assert (response == "y");
	}
	try
	{
		m_database_connection->open(filename.c_str());
	}
	catch (SQLiteException&)
	{
		cout << "Could not open file \"" << filename << "\"." << endl;
		return 1;
	}
	m_database_connection->setup();
	m_main_menu->present_to_user();	
	return 0;
}
	
void PhatbooksTextSession::wrap_up()
{
	return;
}


}  // namespace phatbooks
