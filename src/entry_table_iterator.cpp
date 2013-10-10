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


#include "entry_table_iterator.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include <jewel/assert.hpp>
#include <sstream>

using std::ostringstream;

namespace phatbooks
{

EntryTableIterator
make_date_ordered_actual_ordinary_entry_table_iterator
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	ostringstream oss;
	oss << "select entry_id from entries inner join ordinary_journal_detail "
		<< "using(journal_id) join journals using(journal_id) "
		<< "where transaction_type_id != "
		<< static_cast<int>(non_actual_transaction_type())
		<< " order by date";
	return EntryTableIterator
	(	p_database_connection,
		oss.str()
	);
}


}  // namespace phatbooks
