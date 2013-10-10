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


#include "account_table_iterator.hpp"

namespace phatbooks
{

AccountTableIterator
make_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return AccountTableIterator
	(	p_database_connection,
		"select account_id from accounts order by name"
	);
}

AccountTableIterator
make_type_name_ordered_account_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	return AccountTableIterator
	(	p_database_connection,
		"select account_id from accounts order by account_type_id, name"
	);
}





}  // namespace phatbooks
