#ifndef GUARD_b_string_hpp
#define GUARD_b_string_hpp

#include <wx/string.h>
#include <string>



/**
 * BString is the type of string used at the "business layer" of
 * Phatbooks. It will probably always just be a typedef for some
 * other type of string. We have conversion functions here to
 * convert from BString to UTF-8 std::string and back, and to convert
 * from BString to wxString and back. These may or may not be
 * no-ops, but we should use them in the rest of the code
 * regardless. The idea is that if we ever want to change the
 * type of BString, we should only have to change this file.
 *
 * @todo Testing and documentation.
 */


namespace phatbooks
{

#define PHATBOOKS_USING_WX_STRING_AS_B_STRING 1

#if PHATBOOKS_USING_WX_STRING_AS_B_STRING
	typedef wxString BString;
#else
	typedef std::string BString;
#endif


/**
 * Convert from BString to std::string.
 */
std::string bstring_to_std8(BString const& bs);

/**
 * Convert from std::string to BString.
 */
BString std8_to_bstring(std::string const& s);

/**
 * Convert from BString to wxString.
 */
wxString bstring_to_wx(BString const& wxs);

/**
 * Convert from wxString to BString.
 */
BString wx_to_bstring(wxString const& wxs);


}  // namespace phatbooks



#endif  // GUARD_b_string_hpp
