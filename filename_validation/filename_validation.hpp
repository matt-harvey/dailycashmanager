#ifndef GUARD_filename_validation_hpp
#define GUARD_filename_validation_hpp

#include <string>

namespace phatbooks
{

/**
 * Tests validity of filename (not whole filepath), as to whether it can
 * serve as the name of a Phatbooks user database file.
 *
 * @param s the string that we want to test as to whether it is a valid
 * filename.
 *
 * @param message An error message will be output here if and only if the
 * function returns \e false.
 *
 * @param extension_is_explicit should be true if \e s is supposed to be the
 * whole filename including the extension; the function will then test whether
 * the extension is valid. It should be false if \e s is
 * supposed to omit the extension. The function will then test the whole
 * string on the assumption that it excludes the extension. Thus if
 * \e extension_is_explicit is set to \e false, then "lkajsdf.las" will
 * be \e not be treated as if ".las" is an extension. Rather it will be tested
 * only as to whether it is a valid filename \e base, assuming the correct
 * Phatbooks extension will later be added.
 *
 * @returns \e true if and only if this filename may be used to identify
 * a Phatbooks database file. An error message is stored in \e message if
 * and only if \e false is returned.
 *
 * @todo HIGH PRIORITY Testing.
 */
bool is_valid_filename
(	std::string const& s,
	std::string& message,
	bool extension_is_explicit = true
);



}  // namespace phatbooks


#endif  // GUARD_filename_validation_hpp
