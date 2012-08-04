
/** \file text_user_session.cpp
 *
 * \brief Contains source code relating to textual (console based)
 * user interface with Phatbooks application.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "text_user_session.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/bind.hpp>
#include <algorithm>
#include <cassert>
#include <istream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

using boost::bind;
using std::cin;
using std::cout;
using std::istream;
using std::endl;
using std::max;
using std::ostream;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace phatbooks
{




namespace text_interface_utilities
{

string
get_user_input
(	string const& error_message,
	istream& is,
	ostream& os
)
{
	string input;
	while (!getline(is, input))
	{
		assert (!is);
		is.clear();
		assert (is);
		os << error_message;
	}
	return input;
}

}  // namespace text_interface_utilities





void TextUserSession::say_hello()
{
	cout << "Hello!" << endl;
	return;
}

void TextUserSession::print_numbers()
{
	for (int i = 0; i != 5; ++i)
	{
		cout << i << '\t';
	}
	cout << endl;
	return;
}

void TextUserSession::quit()
{
	return;
}

TextUserSession::TextUserSession():
	m_parent_menu(new Menu),
	m_child_menu(new Menu),
	m_database_connection(new PhatbooksDatabaseConnection)
{
#warning unimplemented function body
	// Set up all the Menu objects.
	
	MenuItem say_hello_item
	(	"Say hello",
		bind(&TextUserSession::say_hello, this)
	);
	m_parent_menu->add_item(say_hello_item);

	MenuItem print_numbers_item
	(	"Print some numbers",
		bind(&TextUserSession::print_numbers, this)
	);
	m_parent_menu->add_item(print_numbers_item);

	MenuItem present_child_menu_item
	(	"Go to inner menu",
		bind(&Menu::present_to_user, m_child_menu)
	);
	m_parent_menu->add_item(present_child_menu_item);

	MenuItem quit_item
	(	"Quit",
		bind(&TextUserSession::quit, this),
		false,
		"x"
	);
	m_parent_menu->add_item(quit_item);

	m_child_menu->add_item(say_hello_item);

	MenuItem return_to_parent_menu_item
	(	"Return to previous menu",
		bind(&TextUserSession::quit, this),
		false,
		"x"
	);
	m_child_menu->add_item(return_to_parent_menu_item);

}

TextUserSession::~TextUserSession()
{
}

void
TextUserSession::run()
{
	cout << endl << "Welcome to Phatbooks." << endl << endl;

#warning unimplemented function body

	// Get filename from user.
	// ...
	
	// Open connection with file.
	// ...
	
	// Setup file.
	// ...
	
	// Present the first menu.
	// ...
	m_parent_menu->present_to_user();
	
	// Farewell the user.
	// ...
	
	return;
}


void
TextUserSession::Menu::add_item(MenuItem const& item)
{
	for
	(	ItemContainer::const_iterator it = m_items.begin();
		it != m_items.end();
		++it
	)
	{
		if (item.name() == it->name())
		{
			throw runtime_error("MenuItem with name already in Menu.");
		}
	}
	m_items.push_back(item);
	return;
}


void
TextUserSession::Menu::present_to_user()
{
	using text_interface_utilities::get_user_input;
	typedef vector<string>::size_type vec_sz;
	bool replay_menu = false;

	do
	{
		// Determine how to label each menu item
		int item_num = 1;
		string::size_type max_label_length = 0;
		vector<string> label_vec;
		ItemContainer::iterator it;
		for (it = m_items.begin(); it != m_items.end(); ++it)
		{
			if (it->has_special_label())
			{
				label_vec.push_back(it->special_label());
			}
			else
			{
				ostringstream oss;
				oss << item_num++;
				label_vec.push_back(oss.str());
			}
			max_label_length = max(label_vec.back().size(), max_label_length);
		}

		// Print the menu
		it = m_items.begin();
		cout << endl;
		for (vec_sz i = 0; i != label_vec.size(); ++i, ++it)
		{
			assert (it < m_items.end());
			cout << label_vec[i]
				 << string(max_label_length + 1 - label_vec[i].size(), ' ')
				 << it->name()
				 << endl;
		}

		// Receive and process user input
		for (bool successful = false; !successful; )
		{
			cout << endl << "Enter an option from the above menu: ";
			string input = get_user_input
			(	"\nThere has been an error receiving your input. "
				"Please try again: "
			);
			// See whether input corresponds to any of the item labels,
			// and invoke the item if it does.
			// This simple linear search is fast enough for all but
			// ridiculously large user menus.
			it = m_items.begin();
			for (vec_sz i = 0; i != label_vec.size(); ++i, ++it)
			{
				assert (it < m_items.end());
				if (input == label_vec[i])
				{
					it->invoke();
					replay_menu = it->repeat_menu();
					successful = true;
					break;
				}
			}
			if (!successful) cout << endl << "Please try again.";
		}
	}
	while (replay_menu);

	return;
}



TextUserSession::MenuItem::MenuItem
(	string const& p_name,
	Menu::CallbackType p_callback,
	bool p_repeat_menu,
	string const& p_special_label
):
	m_name(string()),
	m_callback(p_callback),
	m_repeat_menu(p_repeat_menu),
	m_special_label(p_special_label)
{
	if (p_name.empty())
	{
		throw runtime_error("Menu item name is empty.");
	}
	assert (p_name != "");
	m_name = p_name;
}


string
TextUserSession::MenuItem::name() const
{
	return m_name;
}


string
TextUserSession::MenuItem::special_label() const
{
	if (!has_special_label())
	{
		throw runtime_error("Item does not have special label.");
	}
	assert (has_special_label());
	return m_special_label;
}

bool
TextUserSession::MenuItem::repeat_menu() const
{
	return m_repeat_menu;
}

bool
TextUserSession::MenuItem::has_special_label() const
{
	return !m_special_label.empty();
}

void
TextUserSession::MenuItem::invoke()
{
	m_callback();
	return;
}

bool
TextUserSession::MenuItem::operator<(MenuItem const& rhs) const
{
	return this->m_name < rhs.m_name;
}



}  // namespace phatbooks

