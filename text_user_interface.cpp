#include "text_user_interface.hpp"
#include <cassert>
#include <list>
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::list;
using std::string;

namespace phatbooks
{

namespace text_user_interface
{


void
Menu::add_item(string const& p_name, ResponseType p_response)
{
	MenuItem item(p_name, p_response);
	for
	(
		list<MenuItem>::const_iterator it = m_items.begin();
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
	m_items.sort();
	return;
}


Menu::MenuItem::MenuItem(string const& p_name, Menu::ResponseType p_response):
	m_name(string()),
	m_response(p_response)
{
	if (p_name.size() == 0)
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

Menu::ResponseType
Menu::MenuItem::response() const
{
	return m_response;
}

bool
Menu::MenuItem::operator<(MenuItem const& rhs)
{
	return this->m_name < rhs.m_name;
}




}  // namespace text_user_interface


}  // namespace phatbooks

