#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

#include "entry.hpp"
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
 */
class Journal
{
public:
	// TODO Use this rather than ProtoJournal::Id in various
	// places throughout Phatbooks code base, where applicable.
	typedef sqloxx::Id Id;
	
	virtual ~Journal();

	/*
	static std::string primary_table_name();
	static std::string primary_key_name();
	*/
	
	void set_whether_actual(bool p_is_actual);
	void set_comment(std::string const& p_comment);
	void add_entry(Entry& entry);

	std::vector<Entry> const& entries() const;
	std::string comment() const;
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

private:
	virtual std::vector<Entry> const& do_get_entries() const = 0;
	virtual void do_set_whether_actual(bool p_is_actual) = 0;
	virtual void do_set_comment(std::string const& p_comment) = 0;
	virtual void do_add_entry(Entry& entry) = 0;
	virtual std::string do_get_comment() const = 0;
	virtual bool do_get_whether_actual() const = 0;
};
	

}  // namespace phatbooks


#endif  // GUARD_journal_hpp
