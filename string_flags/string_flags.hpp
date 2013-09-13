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
