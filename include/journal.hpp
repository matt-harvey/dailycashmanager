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


#ifndef GUARD_journal_hpp_6157822681664407
#define GUARD_journal_hpp_6157822681664407

#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal_fwd.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <sqloxx/id.hpp>
#include <wx/string.h>
#include <ostream>
#include <vector>
#include <string>


namespace phatbooks
{

// begin forward declarations

class Entry;

// end forward declarations


/**
 * Abstract base Journal class.
 *
 * Class to represent accounting journals.
 * An accounting journal will
 * typically comprise two or more accounting entries, plus some
 * "journal level" (as opposed to "entry level") data such as the date.
 *
 * A journal can be ProtoJournal, an OrdinaryJournal or a DraftJournal
 * A ordinary journal
 * has been reflected in the entity's financial state. A DraftJournal
 * has not, but has been saved for possible future reuse. Some
 * DraftJournal instances have got Repeater instances associated with them. A
 * DraftJournal with Repeater instances represents a recurring transaction.
 * A ProtoJournal is a journal in the process of being constructed by the
 * user. It might serve as a "seed" for either an OrdinaryJournal or a
 * DraftJournal.
 *
 * As well the ordinary/draft distinction, there is also a distinction between
 * \e actual and \e budget journals. An actual journal reflects an actual
 * change in the entity's wealth, whether the physical form of the wealth
 * (for example, by transferring between asset classes), or a dimimution
 * or augmentation in wealth (by spending or earning money). In contrast
 * a budget journal is a "conceptual" allocation or reallocation of wealth
 * in regards to the \e planned purpose to which the wealth will be put. For
 * example, allocating $100.00 of one's earnings to planned expenditure on
 * food represents a budget transaction.
 */
class Journal
{
public:

	Journal();

	Journal(Journal const& rhs);
	Journal(Journal&&);
	Journal& operator=(Journal const&) = delete;
	Journal& operator=(Journal&&) = delete;
	virtual ~Journal();

	void set_transaction_type
	(	TransactionType p_transaction_type
	);
	void set_comment(wxString const& p_comment);

	void push_entry(sqloxx::Handle<Entry> const& entry);
	void remove_entry(sqloxx::Handle<Entry> const& entry);
	void clear_entries();

	std::vector<sqloxx::Handle<Entry> > const& entries();
	wxString comment();

	bool is_actual();

	TransactionType transaction_type();

	jewel::Decimal balance();

	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * NOTE thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced();

	/**
	 * @returns a Decimal being the sum of the amounts of all the
	 * "destination" Entries in the Journal.
	 */
	jewel::Decimal primary_amount();


protected:

	virtual void swap(Journal& rhs);

	virtual std::vector<sqloxx::Handle<Entry> > const& do_get_entries();
	virtual void do_set_transaction_type
	(	TransactionType p_transaction_type
	);
	virtual void do_set_comment(wxString const& p_comment);
	virtual void do_push_entry(sqloxx::Handle<Entry> const& entry);
	virtual void do_remove_entry(sqloxx::Handle<Entry> const& entry);
	virtual void do_clear_entries();
	virtual wxString do_get_comment();
	virtual TransactionType do_get_transaction_type();

	/**
	 * Cause *this to take on the attributes of rhs that would be common
	 * to all types of Journal.
	 *
	 * Thus, for example, where rhs is an OrdinaryJournal, *this does
	 * \e not take on the \e date attribute of rhs, since ProtoJournal and
	 * DraftJournal do not have a \e date attribute.
	 * Note however that the \e id attribute is \e never taken from the
	 * rhs.
	 *
	 * The \e lhs should pass its id and database connection to the
	 * appropriate parameters in the function. The id should be wrapped
	 * in a boost::optional (uninitialized if has_id returns false).
	 *
	 * The dbc and id parameters are required in order to initialize
	 * the Entries as they are added to the lhs.
	 *
	 * Yes this is a bit messy.
	 *
	 * Note a \e deep, rather than shallow copy of the rhs Entries is made.
	 *
	 * Note this does \e not offer the strong guarantee by itself, but is
	 * designed to be called from derived classes which can implement swap
	 * etc.. to enable the strong guarantee.
	 */
	void mimic_core
	(	Journal& rhs,
		PhatbooksDatabaseConnection& dbc,
		boost::optional<sqloxx::Id> id
	);

	void clear_core();

private:
	struct JournalData;
	std::unique_ptr<JournalData> m_data;
};
	
}  // namespace phatbooks


#endif  // GUARD_journal_hpp_6157822681664407
