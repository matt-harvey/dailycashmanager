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

#ifndef GUARD_visibility_hpp_6967687740441266
#define GUARD_visibility_hpp_6967687740441266

namespace dcm
{

/**
 * Represents information about whether some item, e.g. an Account,
 * is hidden or visible.
 */
enum class Visibility: unsigned char
{
	hidden = 0,
	visible,
	num_visibilities  // do not add enumerators below here
};

}  // namespace dcm

#endif  // GUARD_visibility_hpp_6967687740441266
