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

#include "gui/locale.hpp"
#include "app.hpp"
#include <jewel/assert.hpp>
#include <wx/intl.h>

namespace phatbooks
{
namespace gui
{

wxLocale const&
locale()
{
	App* app = dynamic_cast<App*>(wxTheApp);
	JEWEL_ASSERT (app);
	return app->locale();
}

}  // namespace gui
}  // namespace phatbooks
