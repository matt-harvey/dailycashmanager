#include "phatbooks_text_session.hpp"

#include "consolixx.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/bind.hpp>
#include <iostream>

using consolixx::TextSession;
using boost::bind;
using std::cout;
using std::endl;

namespace phatbooks
{




void PhatbooksTextSession::say_hello()
{
	cout << "Hello!" << endl;
	return;
}

void PhatbooksTextSession::print_numbers()
{
	for (int i = 0; i != 5; ++i)
	{
		cout << i << '\t';
	}
	cout << endl;
	return;
}

void PhatbooksTextSession::quit()
{
	return;
}

PhatbooksTextSession::PhatbooksTextSession():
	m_parent_menu(new Menu),
	m_child_menu(new Menu),
	m_database_connection(new PhatbooksDatabaseConnection)
{
#warning unimplemented function body
	// Set up all the Menu objects.
	
	MenuItem say_hello_item
	(	"Say hello",
		bind(&PhatbooksTextSession::say_hello, this)
	);
	m_parent_menu->add_item(say_hello_item);

	MenuItem print_numbers_item
	(	"Print some numbers",
		bind(&PhatbooksTextSession::print_numbers, this)
	);
	m_parent_menu->add_item(print_numbers_item);

	MenuItem present_child_menu_item
	(	"Go to inner menu",
		bind(&Menu::present_to_user, m_child_menu)
	);
	m_parent_menu->add_item(present_child_menu_item);

	MenuItem quit_item
	(	"Quit",
		bind(&PhatbooksTextSession::quit, this),
		false,
		"x"
	);
	m_parent_menu->add_item(quit_item);

	m_child_menu->add_item(say_hello_item);

	MenuItem return_to_parent_menu_item
	(	"Return to previous menu",
		bind(&PhatbooksTextSession::quit, this),
		false,
		"x"
	);
	m_child_menu->add_item(return_to_parent_menu_item);

}

void PhatbooksTextSession::run()
{
	m_parent_menu->present_to_user();
	return;
}




}  // namespace phatbooks
