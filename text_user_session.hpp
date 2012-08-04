#ifndef GUARD_text_user_session_hpp
#define GUARD_text_user_session_hpp

/** \file text_user_session.hpp
 *
 * \brief Header file for textual (console based) user interface with
 * Phatbooks application
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include <istream>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace phatbooks
{

// FORWARD DECLARATIONS
class PhatbooksDatabaseConnection;

// NON-MEMBER FUNCTION DECLARATIONS

/**
 * Namespace for housing non-member functions that
 * facilitate a textual, console based user interface.
 */
namespace text_interface_utilities
{

/**
 * Function for safely getting a line of console input from a user.
 *
 * @param error_message Message to display to user if there is an error
 * receiving their input.
 *
 * @param is Input stream from which to accept input. Defaults to \c std::cin.
 *
 * @param os Output stream to which error message is printed in the event of
 * an input error. Defaults to \c std::cout.
 *
 * @returns text entered by the user
 *
 * @todo Determine and document throwing behaviour.
 */
std::string get_user_input
(	std::string const& error_message,
	std::istream& is = std::cin,
	std::ostream& os = std::cout
);

}  // namespace text_interface_utilities

// CLASS DEFINITIONS

/**
 * Class representing a user session with a textual (console) interface.
 */
class TextUserSession:
	private boost::noncopyable
{
public:

	TextUserSession();

	~TextUserSession();

	/**
	 * Run the session, presenting text to the user and receiving
	 * and processing text from the user.
	 *
	 * @todo Implement it!
	 */
	void run();

private:
	class Menu;
	class MenuItem;
	// Various Menu objects.

#warning dummy members follow
	boost::shared_ptr<Menu> m_parent_menu;
	boost::shared_ptr<Menu> m_child_menu;
	void say_hello();
	void print_numbers();
	void quit();
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
};




/**
 * Class representing textual menu of user options
 *
 * @todo Write constructor by which you give the menu a title.
 */
class TextUserSession::Menu:
	private boost::noncopyable
{
public:

	// Default destructor is fine.
   
	/**
	 * Add an "item" to the menu.
	 *
	 * @param item a MenuItem.
	 *
	 * @throws std::runtime_error if a
	 * MenuItem with the same banner as \c item banner already exists in the
	 * Menu.
	 *
	 * @throws std::runtime_error if an item with the same special label is
	 * already present in this menu.
	 */
	void add_item(MenuItem const& item);

	/**
	 * Present menu to user, request user input, and respond accordingly
	 *
	 * @todo Document throwing behaviour, if any.
	 */
	void present_to_user();	

private:
	typedef std::vector<MenuItem> ItemContainer;
	ItemContainer m_items;
};




/**
 * Class representing an item in a menu of user options.
 */
class TextUserSession::MenuItem
{
public:

	/**
	 * Type of function to be passed to MenuItem constructor.
	 */
	typedef boost::function< void() > CallbackType;

	 // Default destructor, copy constructor and assignment operator should
	 // be OK.

	/**
	 * @param p_banner non-empty string describing option to the user
	 *
	 * @param p_callback pointer to function to be called on selection of
	 * the option by the user
	 *
	 * @param p_special_label An optional special label by which the item
	 * is "keyed" in the menu. The user will enter this label to select the
	 * item. If no special label is identified, the item will be presented to
	 * the user with a numeric label based on its ordering in the menu. If a
	 * special label is provided, it must contain at least one non-digit
	 * character (to avoid possible confusion with automatically generated
	 * special labels).
	 *
	 * @param p_repeats_menu indicates the desired behaviour after the menu
	 * item has been selected by the user and its callback function
	 * has finished executing. \c true indicates that the same menu in which
	 * the item appeared, should again be presented to the user. \c false
	 * indicates that execution should continue past this point.
	 * This parameter is \c true by default.
	 *
	 * @throws std::runtime_error if \c p_banner is empty
	 *
	 * @throws std::runtime_error if \c p_special_label is non-empty but
	 * contains only digits.
	 */
	MenuItem
	(	std::string const& p_banner,
		CallbackType p_callback,
		bool p_repeats_menu = true,
		std::string const& p_special_label = ""
	);

	/**
	 * @returns string describing MenuItem to user
	 */
	std::string banner() const;

	/**
	 * @returns special label by which the Item will
	 * be identified in the menu, and using which the user will select the
	 * Item.
	 *
	 * @throws std::runtime_error if the Item does not have a special label.
	 */
	std::string special_label() const;

	/**
	 * @return \c true if and only if the menu in which this item appears
	 * should be presented to the user after the item's callback has
	 * finished executing.
	 */
	bool repeats_menu() const;

	/**
	 * @returns \c true if and only if the Item has a special label
	 *
	 * Does not throw.
	 */
	bool has_special_label() const;
	
	/**
	 * Call function stored in MenuItem (whatever function was passed to
	 * p_callback). The throwing behaviour is just the throwing behaviour
	 * of p_callback - no exception is thrown by the \c invoke function in
	 * its own right.
	 */
	void invoke();

private:

	bool operator<(MenuItem const& rhs) const;

	std::string m_banner;
	CallbackType m_callback;
	bool m_repeats_menu;
	std::string m_special_label;
};



// NON-MEMBER FUNCTIONS






}  // namespace phatbooks

#endif  // GUARD_text_user_session_hpp
