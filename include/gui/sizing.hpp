// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_sizing_hpp_3696029835239067
#define GUARD_sizing_hpp_3696029835239067


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

/**
 * @returns a standardized "medium" width suitable, for example, for
 * a DecimalTextCtrl. This is guaranteed to be an even integer.
 */
int medium_width();

/**
 * @returns a standardized "large" width suitable, for example, for
 * a wxTextCtrl for entering an Entry comment of a few words.
 */
int large_width();

/**
 * @returns a standardized "extra large" width suitable, for example,
 * for a wxTextCtrl for entering a comment that's a bit longer than
 * what would fit if one that is \e large_width() wide.
 */
int extra_large_width();

/**
 * @returns a width that is supposed to be as wide as, or slightly wider than,
 * a vertical scrollbar.
 */
int scrollbar_width_allowance();

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_sizing_hpp_3696029835239067
