#ifndef GUARD_text_session_hpp
#define GUARD_text_session_hpp

#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <istream>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>


/**
 * The \c consolixx namespace contains a suite of tools for creating
 * text/console based user interfaces.
 */
namespace consolixx
{



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
	
	explicit Menu
	(	std::string const& p_prompt =
			"Enter an option from the above menu: "
	);
	
   
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
	void add_item(boost::shared_ptr<MenuItem const> const& item);

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

	History m_selection_record;
	std::string m_prompt;

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
	 
	 // This function literally does nothing. It is provided as a default
	 // callback function - see MenuItem constructor.
	 static void do_nothing();

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
	 *
	 * @throws consolixx::MenuItemBannerException if \c p_banner is empty
	 *
	 * @throws consolixx::MenuItemLabelException if \c p_special_label is
	 * non-empty but contains only digits.
	 */
	MenuItem
	(	std::string const& p_banner,
		CallbackType p_callback = do_nothing,
		bool p_repeats_menu = false,
		std::string const& p_special_label = ""
	);

	/**
	 * Provide a MenuItem that does nothing, but revert to the
	 * previous menu, and has "x" as its symbol.
	 *
	 * @todo This sort of sucks. This should probably be taken care
	 * of by the Menu class.
	 */
	static boost::shared_ptr<MenuItem> provide_menu_exit
	(	std::string const& message = "Return to previous menu"
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




// Inline functions

inline
void
TextSession::MenuItem::do_nothing()
{
	return;
}

}  // namespace consolixx



#endif  // GUARD_text_session_hpp
