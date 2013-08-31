// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_string_conv_hpp
#define GUARD_string_conv_hpp

#include <wx/string.h>
#include <string>

namespace phatbooks
{

// DECLARATIONS

/**
 * Convert from std::string to wxString.
 */
wxString std8_to_wx(std::string const& s);

/**
 * Convert from wxString to std::string.
 */
std::string wx_to_std8(wxString const& wxs);


// IMPLEMENTATIONS

inline
wxString
std8_to_wx(std::string const& s)
{
	return wxString::FromUTF8(s.c_str());
}

inline
std::string
wx_to_std8(wxString const& wxs)
{
	return std::string(wxs.utf8_str());
}

}  // namespace phatbooks

#endif  // GUARD_string_conv_hpp
