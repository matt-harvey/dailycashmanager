#ifndef GUARD_text_user_interface_hpp
#define GUARD_text_user_interface_hpp

#include <map>
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
	 * @param str string describing option to the user
	 * @param response pointer to function to be called on selection of
	 * the option by the user
	 */
	void add_item(std::string const& str, ResponseType response);

private:
	typedef std::map<std::string, ResponseType> MapType;
	MapType m_map;
};



}  // namespace text_user_interface

}  // namespace phatbooks

#endif  // GUARD_text_user_interface_hpp
