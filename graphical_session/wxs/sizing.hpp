#ifndef GUARD_sizing_hpp
#define GUARD_sizing_hpp


/**
 * @file sizing.hpp
 *
 * Utilities for storing standard sizes used in widgets.
 */

namespace phatbooks
{
namespace gui
{

/**
 * @returns standard gap to be placed between sub-widgets within their
 * parent widget.
 */
int standard_gap();

/**
 * @returns standard border to be placed around the outside of a set of
 * sub-widgets, within their parent widget.
 */
int standard_border();

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_sizing_hpp
