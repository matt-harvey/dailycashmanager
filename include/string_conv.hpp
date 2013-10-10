/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
