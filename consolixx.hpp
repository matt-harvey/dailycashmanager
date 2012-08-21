#ifndef GUARD_consolixx_hpp
#define GUARD_consolixx_hpp

/** \file consolixx.hpp
 *
 * \brief Header file for \c consolixx - tools for creating text/console
 * based user interfaces.
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
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


/**
 * The \c consolixx namespace contains a suite of tools for creating
 * text/console based user interfaces.
 */
namespace consolixx
{






// NON-MEMBER FUNCTION DECLARATIONS

/**
 * Function for safely getting a line of console input from a user.
 *
 * @param error_message Message to display to user if there is an error
 * receiving their input.
 *
 * @param is Input stream from which to accept input.
 *
 * @param os Output stream to which error message is printed in the event of
 * an input error.
 *
 * @returns text entered by the user
 *
 * @todo Determine and document throwing behaviour.
 */
std::string get_user_input
(	std::string const& error_message =
		"\nThere has been an error reading your input. Please try again: ",
	std::istream& is = std::cin,
	std::ostream& os = std::cout
);

/**
 * Function for safely getting a line of console input from a user, where the
 * input is required to satisfy a given criterion. The user is repeatedly
 * prompted until they provide "satisactory" input, or else (if this option is
 * provided) escape the process by entering a string that satisfies
 * an "escape criterion".
 *
 * @param criterion Boolean test that must return true when applied to the
 * user's input for it to be regarded as "satisfactory".
 * 
 * @param message_on_invalid_input Message to display if user's input is
 * successfully read, but it does not satisfy \c criterion.
 *
 * @param user_can_escape Boolean indicating whether the user has the option
 * to escape the operation by entering a string that satisfies
 * \c escape_criterion.
 *
 * @param escape_criterion Boolean test such that, if it returns true, the
 * user will escape this procedure. Note this takes higher priority over
 * \c criterion when determining the response to the user's input.
 *
 * @param escape_message String to display if the user successfully escapes
 * the procedure.
 *
 * @param return_string_on_escape String to return in the event the user
 * escapes the procedure.
 *
 * @param message_on_unsuccessful_read Message to display if the read
 * operation itself fails. (If this happens it means no user input has even
 * been received that might be checked against the criterion.)

 * @param is Input stream from which to accept input.
 * 
 * @param os Output stream to which messages are to printed in the event of
 * input error or input that does not satisfy the criterion.
 *
 * Note that by default, newlines are \e not printed after any of the error
 * messages or etc. displayed to the user.
 */
std::string get_constrained_user_input
(	boost::function< bool(std::string const&) > criterion,
	std::string const& message_on_invalid_input,
	bool user_can_escape = true,
	boost::function< bool(std::string const&) > escape_criterion =
		boost::bind(&std::string::empty, _1),
	std::string const& escape_message = "\nProcedure aborted.\n",
	std::string const& return_string_on_escape = "",
	std::string const& message_on_unsuccessful_read =
		"\nThere has been an error reading your input. Please try again: ",	
	std::istream& is = std::cin,
	std::ostream& os = std::cout
);



// CLASS DEFINITIONS

/**
 * Class representing a user session with a textual (console) interface.
 * Inherit from this class to create a project-specific text user session
 * class.
 */
class TextSession:
	private boost::noncopyable
{
public:

	virtual ~TextSession();

protected:
	class Menu;
	class MenuItem;
};




/**
 * Class representing textual menu of user options
 * This class is not designed to be extended.
 *
 * When the user selects a MenuItem from the Menu, the selection is
 * "remembered" in the menu.
 *
 * @todo Write constructor by which you give the menu a title.
 */
class TextSession::Menu:
	private boost::noncopyable
{
public:

   Menu();
   
	/**
	 * Add an "item" to the menu.
	 *
	 * @param a shared pointer to a const MenuItem
	 *
	 * @throws consolixx::MenuItemBannerException if a
	 * MenuItem with the same banner as \c item banner already exists in the
	 * Menu.
	 *
	 * @throws consolixx::MenuItemLabelException if an item with the same
	 * special label is already present in this menu.
	 */
	void add_item(boost::shared_ptr<MenuItem const> item);

	/**
	 * Present menu to user, request user input, and respond accordingly
	 *
	 * @todo Document throwing behaviour, if any.
	 */
	void present_to_user();	

	/**
	 * @returns the last selected MenuItem.
	 *
	 * @throws consolixx::NoMenuHistoryException if the MenuItem doesn't have
	 * any history yet (i.e. the user has not yet made any selection from
	 * the menu).
	 */
	boost::shared_ptr<MenuItem const> last_choice() const;

private:

	typedef
		std::vector< boost::shared_ptr<MenuItem const> >
		ItemContainer;

	ItemContainer m_items;

	typedef
		boost::circular_buffer< boost::shared_ptr<MenuItem const> >
		History;

	History m_recent_choices;

};




/**
 * Class representing an item in a menu of user options.
 * This class is not designed to be extended.
 */
class TextSession::MenuItem
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
	 * @throws consolixx::MenuItemBannerException if \c p_banner is empty
	 *
	 * @throws consolixx::MenuItemLabelException if \c p_special_label is
	 * non-empty but contains only digits.
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
	 * @throws consolixx::MenuItemLabelException if the MenuItem does not
	 * have a special label.
	 */
	std::string special_label() const;

	/**
	 * @return \c true if and only if the menu in which this item appears
	 * will be presented to the user after the item's callback has
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
	void invoke() const;

private:
	std::string m_banner;
	CallbackType m_callback;
	bool m_repeats_menu;
	std::string m_special_label;
};




}  // namespace consolixx

#endif  // GUARD_consolixx_hpp
