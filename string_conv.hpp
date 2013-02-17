#ifndef GUARD_string_conv_hpp
#define GUARD_string_conv_hpp

#include <wx/string.h>
#include <string>

/**
 * @file string_conv.hpp
 *
 * Contains function for converting back and forth
 * between wxString and UTF-8-encoded std::string
 *
 * @todo Testing and documentation.
 */


namespace phatbooks
{
namespace string_conv
{

// DECLARATIONS

std::string wx_to_std8(wxString const& wxs);

wxString std8_to_wx(std::string const& s);

// IMPLEMENATIONS

inline
std::string
wx_to_std8(wxString const& wxs)
{
	return std::string(wxs.utf8_str());
}

inline
wxString
std8_to_wx(std::string const& s)
{
	return wxString::FromUTF8(s.c_str());
}


}  // namespace string_conv
}  // namespace phatbooks

#endif  // GUARD_string_conv_hpp
