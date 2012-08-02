#ifndef GUARD_text_user_interface_hpp
#define GUARD_text_user_interface_hpp

#include <list>
#include <string>
#include <boost/noncopyable.hpp>


namespace phatbooks
{

/** 
 * Namespace to hold code relating to a text user interface for Phatbooks
 *
 * This namespace may hold various ordinary (non-class-member) functions, in
 * addition to object-oriented code. This
 * namespace is therefore provided to organise these functions together
 * despite not being within a class. It is also designed to house all user
 * interface oriented code where the interface is text/console based. It is
 * intended that a graphical user interface will also be provided in due
 * course.
 */
namespace text_user_interface
{




// CLASS DEFINITIONS

/**
 * Class representing textual menu of user options
 */
class Menu:
	private boost::noncopyable
{
public:

	typedef void (*ResponseType)();
	
	// Default constructor and destructor are fine
   
	/**
	 * Add an "item" to the menu, consisting of a string for presentation
	 * to the user, describing the option being presented, and a pointer
	 * to a function to be called when the user selects that option.
	 *
	 * @todo document throwing behaviour. Should throw if string is
	 * empty, or if string the string of an item already in the Menu.
	 *
	 * @param str string describing option to the user
	 * @param response pointer to function to be called on selection of
	 * the option by the user
	 */
	void add_item(std::string const& name, ResponseType response);

private:
	struct MenuItem;
	std::list<MenuItem> m_items;
};

/**
 * Class representing an item in a menu of user options.
 * This class should only be accessible by the Menu class.
 */
class Menu::MenuItem
{
public:

	 // Default destructor, copy constructor and assignment operator should
	 // be OK.

	/**
	 * @param p_str non-empty string describing option to the user
	 * @param p_response pointer to be called when the user
	 * selects this option
	 * @throws std::runtime_error if \c str is empty
	 */
	MenuItem(std::string const& p_name, Menu::ResponseType p_response);

	/**
	 * @returns string describing MenuItem to user
	 */
	std::string name() const;
	
	/**
	 * @returns pointer to function called when user selects this
	 * option
	 *
	 * Doesn't throw.
	 */
	Menu::ResponseType response() const;

	bool operator<(MenuItem const& rhs);

private:
	std::string m_name;
	Menu::ResponseType m_response;
};



// NON-MEMBER FUNCTIONS





}  // namespace text_user_interface

}  // namespace phatbooks

#endif  // GUARD_text_user_interface_hpp
