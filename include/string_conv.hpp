/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_string_conv_hpp_06682258568090507
#define GUARD_string_conv_hpp_06682258568090507

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

#endif  // GUARD_string_conv_hpp_06682258568090507
