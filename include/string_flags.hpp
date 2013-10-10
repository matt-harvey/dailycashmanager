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


#ifndef GUARD_string_flags_hpp_04174686784006044
#define GUARD_string_flags_hpp_04174686784006044

#include <jewel/flag_set.hpp>

namespace phatbooks
{

/**
 * Boolean flags for indicating data about features of a string
 * which is to be processed in some way.
 * This could also include both natural language strings and numeric
 * strings.
 */
namespace string_flags
{
	enum StringFlags
	{
		capitalize			  = (1 << 0),  // capitalize
		include_article		  = (1 << 1),  // use indefinite article
		pluralize			  = (1 << 2),  // use plural form
		dash_for_zero		  = (1 << 3),  // allow '-' to mean zero
		allow_negative_parens = (1 << 4),  // allow parentheses for negative
		hard_align_right	  = (1 << 5),  // always align hard to the right,
		                                   // without any padding
	};

}  // namespace string_flags



}  // namespace phatbooks

#endif  // GUARD_string_flags_hpp_04174686784006044
