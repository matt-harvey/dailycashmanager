/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
