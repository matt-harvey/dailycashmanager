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

#ifndef GUARD_transaction_side_hpp_6396077878957651
#define GUARD_transaction_side_hpp_6396077878957651

namespace phatbooks
{

/**
 * Represents the notion of a particular side of a
 * Journal, being either the "source" or the "destination"
 * side. "Source" transactions are usually CR and "destination"
 * usually DR; but this need not always be the case. In particular,
 * the signs may be the other way around in case we are reversing a
 * previous transaction.
 */
enum class TransactionSide: unsigned char
{
	source = 0,
	destination,
	num_transaction_sides  // do not insert enumerators below here
};

}  // namespace phatbooks


#endif  // GUARD_transaction_side_hpp_6396077878957651
