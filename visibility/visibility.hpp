// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_visibility_hpp_6967687740441266
#define GUARD_visibility_hpp_6967687740441266

namespace phatbooks
{

/**
 * Houses Visibility enum.
 */
namespace visibility
{

/**
 * Represents information about whether some item, e.g. an Account,
 * is hidden or visible.
 */
enum Visibility
{
	hidden = 0,
	visible,
	num_visibilities  // do not add enumerators below here
};

}  // namespace visibility

}  // namespace phatbooks

#endif  // GUARD_visibility_hpp_6967687740441266
