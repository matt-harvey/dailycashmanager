// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_locale_hpp_4724053314292828
#define GUARD_locale_hpp_4724053314292828

#include <wx/intl.h>

namespace phatbooks
{
namespace gui
{

/**
 * Convenience function returning a reference to the current wxLocale.
 * Instance of phatbooks::gui::App should have been initialized before
 * calling this function, or behaviour will be undefined.
 */
wxLocale const& locale();

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_locale_hpp_4724053314292828
