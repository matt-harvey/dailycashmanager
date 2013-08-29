#ifndef GUARD_visibility_hpp
#define GUARD_visibility_hpp

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

#endif  // GUARD_visibility_hpp
