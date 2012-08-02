#include "text_user_interface.hpp"


namespace phatbooks
{

namespace text_user_interface
{

void Menu::add_item(std::string const& str, ResponseType response)
{
	m_map[str] = response;
	return;
}




}  // namespace text_user_interface

}  // namespace phatbooks

