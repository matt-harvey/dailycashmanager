#include "filepath_validator.hpp"
#include "windows_macro.hpp"
#include <boost/regex.hpp>
#include <string>

using boost::regex;
using boost::regex_match;
using std::string;

namespace phatbooks
{

bool
is_valid_filepath(string const& s)
{
	regex const target_regex;	
#	if PHATBOOKS_ON_WINDOWS
		target_regex = // TODO
#	else
		target_regex = // TODO
#	endif
	return regex_match(s, target_regex);
}




}  // namespace phatbooks
