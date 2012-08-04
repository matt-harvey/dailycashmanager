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


#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace phatbooks
{

// FORWARD DECLARATIONS
class PhatbooksDatabaseConnection;

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
	boost::shared_ptr<Menu> m_dummy_menu;
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

	/**
	 * Type of function to be passed to MenuItem constructor.
	 * The function should return \c true to indicate that
	 * the Menu in which it appears should again be presented
	 * to the user after it has been invoked; or \c false to
	 * indicate that the menu in which it appears should not again
	 * be presented.
	 */
	typedef boost::function< void() > CallbackType;

	// Default destructor is fine.
   
	/**
	 * Add an "item" to the menu.
	 *
	 * @param item a MenuItem.
	 *
	 * @throws std::runtime_error if a
	 * MenuItem with the same name as \c item name already exists in the Menu.
	 *
	 * @todo Make it throw if an Item with this special label already
	 * exists in the Menu.
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
 * This class should only be accessible by the Menu class.
 */
class TextUserSession::MenuItem
{
public:

	 // Default destructor, copy constructor and assignment operator should
	 // be OK.

	/**
	 * @param p_str non-empty string describing option to the user
	 *
	 * @param p_callback pointer to function to be called on selection of
	 * the option by the user - or any callable type with a compatible
	 * signature.
	 *
	 * @param p_special_label is an optional special label by which the item
	 * is "keyed" in the menu. The user will enter this label to select the
	 * item. If no special label is identified, the item will be presented to
	 * the user with a numeric label based on its ordering in the menu.
	 * selects this option
	 *
	 * @param p_repeat_menu indicates the desired behaviour after the menu
	 * item has been selected by the user and its callback function
	 * has finished executing. \true indicates that the same menu in which
	 * the item appeared, should again be presented to the user. \false
	 * indicates that execution should continue past this point.
	 * This parameter is \true by default.
	 *
	 * @throws std::runtime_error if \c str is empty
	 */
	MenuItem
	(	std::string const& p_name,
		Menu::CallbackType p_callback,
		bool p_repeat_menu = true,
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
	 * @return \c true if and only if the menu in which this item appears
	 * should be presented to the user after the item's callback has
	 * finished executing.
	 */
	bool repeat_menu() const;

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

	bool operator<(MenuItem const& rhs) const;

private:
	std::string m_name;
	Menu::CallbackType m_callback;
	bool m_repeat_menu;
	std::string m_special_label;
};



// NON-MEMBER FUNCTIONS






}  // namespace phatbooks

#endif  // GUARD_text_user_session_hpp
