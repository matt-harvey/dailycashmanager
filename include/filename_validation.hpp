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

#ifndef GUARD_filename_validation_hpp_29534295773848096
#define GUARD_filename_validation_hpp_29534295773848096

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
 * a Phatbooks database file. An error message will be assigned to \e message
 * if and only if \e false is returned.
 */
bool is_valid_filename
(	std::string const& s,
	std::string& message,
	bool extension_is_explicit = true
);



}  // namespace phatbooks


#endif  // GUARD_filename_validation_hpp_29534295773848096
