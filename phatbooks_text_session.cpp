#include "phatbooks_text_session.hpp"

#include "consolixx.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

using consolixx::get_user_input;
using consolixx::get_constrained_user_input;
using consolixx::TextSession;
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
	
	cout << "Enter a three-letter word: ";
	get_constrained_user_input
	(	has_three_letters,
		"Try again. Enter a three-letter word: ",
		false
	);


	/*
	string filename = elicit_filename();
	run(filename);
	*/
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

string PhatbooksTextSession::elicit_filename()
{
	cout << "Enter name of file to open (or to create if it doesn't already "
	        "exist): ";
	string filename = get_user_input();
	return filename;
}
	
void PhatbooksTextSession::wrap_up()
{
	return;
}


}  // namespace phatbooks
