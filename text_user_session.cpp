
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
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using boost::bind;
using std::cin;
using std::cout;
using std::endl;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace phatbooks
{


void TextUserSession::say_hello()
{
	cout << "Hello!" << endl;
	m_dummy_menu->present_to_user();
	return;
}

void TextUserSession::print_numbers()
{
	for (int i = 0; i != 5; ++i)
	{
		cout << i << '\t';
	}
	cout << endl;
	m_dummy_menu->present_to_user();
	return;
}

void TextUserSession::quit()
{
	cout << "Exiting application..." << endl;
	return;
}

TextUserSession::TextUserSession():
	m_dummy_menu(new Menu),
	m_database_connection(new PhatbooksDatabaseConnection)
{
#warning unimplemented function body
	// Set up all the Menu objects.
	m_dummy_menu->add_item
	(	"Say hello",
		bind(&TextUserSession::say_hello, this)
	);
	m_dummy_menu->add_item
	(	"Print some numbers",
		bind(&TextUserSession::print_numbers, this)
	);
	m_dummy_menu->add_item
	(	"Quit",
		bind(&TextUserSession::quit, this)
	);
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
	m_dummy_menu->present_to_user();
	
	// Farewell the user.
	// ...
	
	return;
}


void
TextUserSession::Menu::add_item
(	string const& p_name,
	ResponseType p_response,
	string const& p_special_label
)
{
	MenuItem item(p_name, p_response, p_special_label);
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
	// Determine how to label each menu item
	int item_number = 1;
	string::size_type max_label_length = 0;
	vector<string> label_vec;
	for
	(	ItemContainer::const_iterator it = m_items.begin();
		it != m_items.end();
		++it
	)
	{
		string label;
		if (it->has_special_label())
		{
			label = it->special_label();
		}
		else
		{	
			ostringstream oss;
			oss << item_number;
			label = oss.str();
			++item_number;
		}
		if (label.size() > max_label_length)
		{
			max_label_length = label.size();
		}
		label_vec.push_back(label);
	}

	// Print the menu
	ItemContainer::const_iterator items_iter = m_items.begin();
	for (vector<string>::size_type i = 0; i != label_vec.size(); ++i)
	{
		assert (i != m_items.size());
		assert (items_iter != m_items.end());
		cout << label_vec[i]
		     << string(max_label_length + 1 - label_vec[i].size(), ' ')
			 << items_iter->name()
			 << endl;
		++items_iter;
	}
	cout << endl << "Enter an option from the above menu: ";
	
	// Get user input
	string input;
	bool invocation_successful = false;
	do
	{
		bool input_successful = getline(cin, input);
		if (input_successful)
		{
			// Find if input corresponds to an item label
			ItemContainer::iterator items_iter2 = m_items.begin();
			for (vector<string>::size_type i = 0; i != label_vec.size(); ++i)
			{
				assert (items_iter2 != m_items.end());
				if (input == label_vec[i])
				{
					// User has selected one of the items.
					items_iter2->invoke();
					invocation_successful = true;
				}
				++items_iter2;
			}
			assert (items_iter2 == m_items.end());
			if (!invocation_successful)
			{
				cout << "Your input does not match any of the above items. "
					 << "Please try again."
					 << endl
					 << "Enter an option from the above menu: ";
			}
		}
		else
		{
			// There was an error receiving input
			assert (!cin);
			cin.clear();
			assert (cin);
			cout << "There has been an error receiving your input. "
				 << "Please try again: " << endl;
		}
	}
	while (!invocation_successful);

	return;
}



TextUserSession::Menu::MenuItem::MenuItem
(	string const& p_name,
	Menu::ResponseType p_response,
	string const& p_special_label
):
	m_name(string()),
	m_response(p_response),
	m_special_label(p_special_label)
{
	if (p_name.empty())
	{
		throw runtime_error("p_name is empty.");
	}
	assert (p_name.size() > 0);
	m_name = p_name;
}


string
TextUserSession::Menu::MenuItem::name() const
{
	return m_name;
}


string
TextUserSession::Menu::MenuItem::special_label() const
{
	if (m_special_label.empty())
	{
		throw runtime_error("Item does not have special label.");
	}
	assert (!m_special_label.empty());
	return m_special_label;
}

bool
TextUserSession::Menu::MenuItem::has_special_label() const
{
	return !m_special_label.empty();
}

void
TextUserSession::Menu::MenuItem::invoke() const
{
	m_response();
	return;
}

bool
TextUserSession::Menu::MenuItem::operator<(MenuItem const& rhs) const
{
	return this->m_name < rhs.m_name;
}



}  // namespace phatbooks

