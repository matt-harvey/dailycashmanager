#include "filename_validation.hpp"
#include "application.hpp"
#include <boost/regex.hpp>
#include <jewel/on_windows.hpp>
#include <cassert>
#include <string>
#include <vector>

using boost::regex;
using boost::regex_match;
using std::string;

namespace phatbooks
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
	}

	/**
	 * Characters that cannot occur \e anywhere in a filepath.
	 */
	bool is_prohibited_filepath_character(char c)
	{
		return c == '\0';
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
		(	"\\b(?i:con|prn|aux|nul|com[1-9]|lpt[1-9])\\b"
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
	 * @returns base of filename, i.e. filename not including
	 * the extension. Does not include any dot. Note if the filename
	 * starts with a dot, this returns the string after the first dot,
	 * up to but not including the second dot.
	 *
	 * WARNING I actually don't think we need this, so I have commented
	 * it out.
	bool filename_base(string const& s)
	{
		typedef string::const_iterator Iter;
		Iter it = s.begin();
		if (*it == '.')
		{
			Iter rev = s.end();
			while (*rev != '.') --rev;
			assert (rev == '.');
			if (rev == it)
			{
				// There is only one dot and the whole thing is the base
				return s;
			}
			else
			{
				// There are multiple dots.
				return string(it, rev);
			}
		}
		assert (it == s.begin());
		assert (s[0] != '.');
		Iter rev = s.end();
		while (rev != s.begin() && *rev != '.') --rev;
		if (*rev == '.')
		{
			return string(it, rev);
		}
		assert (rev == s.begin());
		assert (rev != '.');
		return s;
	}
	*/

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
		assert (rev != s.begin());
		assert (*rev == '.');
		return string(rev, s.end());
	}

	/**
	 * Filenames that are prohibited for use as Phatbooks files,
	 * due either to not having the right extension, or for other reasons.
	 * Populate \e message with an error message if and only if the return
	 * value is true.
	 *
	 * @returns \e true if and only if the filename is prohibited.
	 */
	bool is_prohibited_phatbooks_filename(string const& s, string& message)
	{
		string const extension = filename_extension(s);
		if (extension != bstring_to_std8(Application::filename_extension()))
		{
			message =
				"Filename must have extension " +
				Application::filename_extension();
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
(	string const& s,
	string& message,
	bool extension_is_explicit
)
{
	if (extension_is_explicit)
	{
		return !is_prohibited_phatbooks_filename(s, message);
	}
	assert (!extension_is_explicit);
	return !is_generally_prohibited_filename(s, message);
}


}  // namespace phatbooks
