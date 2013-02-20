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

// NOTE Changing this typedef entails changing the
// implementations of the conversion functions contained
// in this file.
typedef std::string BString;

std::string bstring_to_std8(BString const& bs);
BString std8_to_bstring(std::string const& s);

wxString bstring_to_wx(BString const& wxs);
BString wx_to_bstring(wxString const& wxs);


// IMPLEMENTATIONS

inline
std::string
bstring_to_std8(BString const& bs)
{
	return bs;
}

inline
BString
std8_to_bstring(std::string const& s)
{
	return s;
}

inline
wxString
bstring_to_wx(BString const& bs)
{
	return wxString::FromUTF8(bs.c_str());
}

inline
BString
wx_to_bstring(wxString const& wxs)
{
	return std::string(wxs.utf8_str());
}


}  // namespace phatbooks



#endif  // GUARD_b_string_hpp
