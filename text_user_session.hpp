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
	Menu* m_dummy_menu;
	void say_hello();
	void print_numbers();
	void quit();

	PhatbooksDatabaseConnection* m_database_connection;
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

	typedef boost::function< void() > ResponseType;

	// Default destructor is fine.
   
	/**
	 * Add an "item" to the menu, consisting of a string for presentation
	 * to the user, describing the option being presented, and a pointer
	 * to a function to be called when the user selects that option.
	 *
	 * @throws std::runtime_error if p_name is an empty string or if a
	 * MenuItem with this name already exists in the Menu.
	 * @todo Make it throw if an Item with this special label already
	 * exists in the Menu.
	 * @param p_name string describing option to the user
	 * @param p_response pointer to function to be called on selection of
	 * the option by the user - or any callable type with a compatible
	 * signature.
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
class TextUserSession::Menu::MenuItem
{
public:

	 // Default destructor, copy constructor and assignment operator should
	 // be OK.

	/**
	 * @param p_str non-empty string describing option to the user
	 *
	 * @param p_response pointer to function to be called on selection of
	 * the option by the user - or any callable type with a compatible
	 * signature.
	 *
	 * @param p_special_label optional special label by which the item will
	 * be "keyed" in the menu. The user will enter this label to select the
	 * item. If no special label is identified, the item will be presented to
	 * the user with a numeric label based on its ordering in the menu.
	 * selects this option
	 *
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
	void invoke() const;

	bool operator<(MenuItem const& rhs) const;

private:
	std::string m_name;
	Menu::ResponseType m_response;
	std::string m_special_label;
};



// NON-MEMBER FUNCTIONS






}  // namespace phatbooks

#endif  // GUARD_text_user_session_hpp
