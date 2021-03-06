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

#include "filename_validation.hpp"
#include "app.hpp"
#include "string_conv.hpp"
#include <boost/regex.hpp>
#include <jewel/assert.hpp>
#include <jewel/on_windows.hpp>
#include <wx/string.h>
#include <string>
#include <vector>

using boost::regex;
using boost::regex_match;
using std::string;

namespace dcm
{

namespace
{
    /**
     * While some of the prohibited characters may be allowed by some
     * operating
     * systems, we want to avoid all of them anyway. It avoids
     * complications as these characters have special meanings for shell
     * scripts or other tools, that we might want to use later on.
     */
    bool is_prohibited_filename_character(char c)
    {
        switch(c)
        {
        case '\0':
        case '/':
        case '\\':
        case '?':
        case '%':
        case '*':
        case ':':
        case '|':
        case '"':
        case '<':
        case '>':
            return true;
        default:
            return false;
        }
        JEWEL_HARD_ASSERT (false);
    }

    /**
     * Filenames that are prohibited, regardless of the extension.
     * Populates \e message with an error message if and only if the
     * return value is \e true.
     *
     * @returns \e true if and only the filename is prohibited.
     */
    bool is_generally_prohibited_filename(string const& s, string& message)
    {
        if (s.empty())
        {
            message = "Filename cannot be empty string.";
            return true;
        }
        for (string::const_iterator it = s.begin(); it != s.end(); ++it)
        {
            if (is_prohibited_filename_character(*it))
            {
                if (*it == '\0')
                {
                    message = "The NULL character ";
                }
                else
                {
                    message = "The character '";
                    message.push_back(*it);
                }
                message += "' cannot be used in a filename.";
                return true;
            }
        }
        regex const windows_special_prohibited_filenames
        (   "\\b(?i:con|prn|aux|nul|com[1-9]|lpt[1-9])\\b"
        );
        if (regex_match(s, windows_special_prohibited_filenames))
        {
            message = "\"" + s + "\" cannot be used as a filename.";
            return true;
        }
        regex const two_dots_filename(".*\\.\\..*");
        if (regex_match(s, two_dots_filename))
        {
            message = "Cannot use two consecutive dots in a filename.";
            return true;
        }
        return false;
    }

    /**
     * @return filename extension, \e including the dot. Can be empty
     * string.
     */
    string filename_extension(string const& s)
    {
        typedef string::const_iterator Iter;
        Iter rev = s.end();
        while (rev != s.begin() && *rev != '.') --rev;
        if (rev == s.begin())
        {
            return "";
        }
        JEWEL_ASSERT (rev != s.begin());
        JEWEL_ASSERT (*rev == '.');
        return string(rev, s.end());
    }

    /**
     * Filenames that are prohibited for use as DCM files,
     * due either to not having the right extension, or for other reasons.
     * Populate \e message with an error message if and only if the return
     * value is true.
     *
     * @returns \e true if and only if the filename is prohibited.
     */
    bool is_prohibited_dcm_filename(string const& s, string& message)
    {
        string const extension = filename_extension(s);
        if (extension != wx_to_std8(App::filename_extension()))
        {
            message =
                "Filename must have extension " +
                App::filename_extension() +
                ".";
            return true;
        }
        if (is_generally_prohibited_filename(s, message))
        {
            return true;
        }
        return false;
    }

}  // end anonymous namespace


bool
is_valid_filename
(   string const& s,
    string& message,
    bool extension_is_explicit
)
{
    if (extension_is_explicit)
    {
        if (s == App::filename_extension())
        {
            message = "Filename cannot consist solely of extension.";
            return false;
        }
        return !is_prohibited_dcm_filename(s, message);
    }
    JEWEL_ASSERT (!extension_is_explicit);
    return !is_generally_prohibited_filename(s, message);
}


}  // namespace dcm
