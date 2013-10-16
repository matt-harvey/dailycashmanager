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
