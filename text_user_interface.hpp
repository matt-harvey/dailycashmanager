#ifndef GUARD_text_user_interface_hpp
#define GUARD_text_user_interface_hpp

#include <vector>
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
 *
 * @todo Write constructor by which you give the menu a title.
 */
class Menu:
	private boost::noncopyable
{
public:

	typedef void (*ResponseType)();

	// Default destructor is fine.
   
	/**
	 * Add an "item" to the menu, consisting of a string for presentation
	 * to the user, describing the option being presented, and a pointer
	 * to a function to be called when the user selects that option.
	 *
	 * @throws std::runtime_error if p_name is an empty string or if a
	 * MenuItem with this name already exists in the Menu.
	 *
	 * @param p_name string describing option to the user
	 * @param p_response pointer to function to be called on selection of
	 * the option by the user
	 */
	void add_item
	(	std::string const& p_name,
		ResponseType p_response,
		std::string const& p_special_label = ""
	);

	/**
	 * Present menu to user, request user input, and respond accordingly
	 *
	 * @todo Document throwing behaviour, if any.
	 * @todo Write part that accepts user input and then calls the
	 * corresponding function - or asks again for input if the input
	 * is invalid.
	 */
	void present_to_user();	

private:
	class MenuItem;
	typedef std::vector<MenuItem> ItemContainer;
	ItemContainer m_items;
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
	 * @param p_special_label optional special label by which the item will
	 * be "keyed" in the menu. The user will enter this label to select the
	 * item. If no special label is identified, the item will be presented to
	 * the user with a numeric label based on its ordering in the menu.
	 * selects this option
	 * @throws std::runtime_error if \c str is empty
	 */
	MenuItem
	(	std::string const& p_name,
		Menu::ResponseType p_response,
		std::string const& p_special_label = ""
	);

	/**
	 * @returns string describing MenuItem to user
	 */
	std::string name() const;

	/**
	 * @returns special label by which the Item will
	 * be identified in the menu, and using which the user will select the
	 * Item.
	 *
	 * @throws std::runtime_error if the Item does not have a special label.
	 */
	std::string special_label() const;

	/**
	 * @returns true iff and only if the Item has a special label
	 *
	 * Does not throw.
	 */
	bool has_special_label() const;
	
	/**
	 * Call function stored in MenuItem (whatever function was passed to
	 * p_response). The throwing behaviour is just the throwing behaviour
	 * of p_response.
	 *
	 * Doesn't throw.
	 */
	void evoke() const;

	bool operator<(MenuItem const& rhs) const;

private:
	std::string m_name;
	Menu::ResponseType m_response;
	std::string m_special_label;
};



// NON-MEMBER FUNCTIONS





}  // namespace text_user_interface

}  // namespace phatbooks

#endif  // GUARD_text_user_interface_hpp
