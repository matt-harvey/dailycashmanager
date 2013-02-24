#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

#include "b_string.hpp"
#include "entry.hpp"
#include "phatbooks_persistent_object.hpp"
#include <ostream>
#include <sqloxx/general_typedefs.hpp>
#include <vector>
#include <string>

namespace jewel
{
	class Decimal;
}  // namespace jewel



namespace phatbooks
{

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

	typedef PhatbooksPersistentObjectBase::Id Id;
		
	virtual ~Journal();

	void set_whether_actual(bool p_is_actual);
	void set_comment(BString const& p_comment);
	void add_entry(Entry& entry);
	void remove_entry(Entry& entry);

	std::vector<Entry> const& entries() const;
	BString comment() const;
	bool is_actual() const;
	jewel::Decimal balance() const;


	/**
	 * @returns true if and only if the journal balances, i.e. the total
	 * of the entries is equal to zero.
	 *
	 * @todo Note, thinking a little about this function shows
	 * that all entries in a journal must be expressed in a common currency.
	 * It doesn't make sense to think of entries in a single journal as being
	 * in different currencies. An entry must have its value frozen in time.
	 */
	bool is_balanced() const;

protected:
	virtual void do_output(std::ostream& os) const;

private:
	virtual std::vector<Entry> const& do_get_entries() const = 0;
	virtual void do_set_whether_actual(bool p_is_actual) = 0;
	virtual void do_set_comment(BString const& p_comment) = 0;
	virtual void do_add_entry(Entry& entry) = 0;
	virtual void do_remove_entry(Entry& entry) = 0;
	virtual BString do_get_comment() const = 0;
	virtual bool do_get_whether_actual() const = 0;

	static void output_journal_aux(std::ostream& os, Journal const& oj);
	
	friend
	std::ostream& operator<<(std::ostream& os, Journal const& oj);
};
	


std::ostream&
operator<<(std::ostream& os, Journal const& oj);




}  // namespace phatbooks


#endif  // GUARD_journal_hpp
