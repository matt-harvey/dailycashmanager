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

#ifndef GUARD_locale_hpp_4724053314292828
#define GUARD_locale_hpp_4724053314292828

#include <wx/intl.h>

namespace dcm
{
namespace gui
{

/**
 * Convenience function returning a reference to the current wxLocale.
 * Instance of dcm::App should have been initialized before
 * calling this function, or behaviour will be undefined.
 */
wxLocale const& locale();

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_locale_hpp_4724053314292828
