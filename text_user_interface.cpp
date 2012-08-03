#include "text_user_interface.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace phatbooks
{

namespace text_user_interface
{


void
Menu::add_item
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
Menu::present_to_user()
{
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
	ItemContainer::const_iterator items_iter = m_items.begin();
	for (vector<string>::size_type i = 0; i != label_vec.size(); ++i)
	{
		assert (i != m_items.size());
		assert (items_iter != m_items.end());
		cout << label_vec[i]
		     << string(max_label_length + 1 - label_vec[i].size(), ' ')
			 << (items_iter++)->name()
			 << endl;
	}
	cout << endl << "Enter an option from the above menu: " << endl;
#warning incomplete function
	// Enter code to get user input
	return;
}
		
		


Menu::MenuItem::MenuItem
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
Menu::MenuItem::name() const
{
	return m_name;
}


string
Menu::MenuItem::special_label() const
{
	if (m_special_label.empty())
	{
		throw runtime_error("Item does not have special label.");
	}
	assert (!m_special_label.empty());
	return m_special_label;
}

bool
Menu::MenuItem::has_special_label() const
{
	return !m_special_label.empty();
}

void
Menu::MenuItem::evoke() const
{
	m_response();
	return;
}

bool
Menu::MenuItem::operator<(MenuItem const& rhs) const
{
	return this->m_name < rhs.m_name;
}




}  // namespace text_user_interface


}  // namespace phatbooks

