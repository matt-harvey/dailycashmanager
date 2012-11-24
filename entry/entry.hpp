#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

#include "journal.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

class Account;
class EntryImpl;

class Entry
{
public:
	typedef sqloxx::Id Id;
	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	explicit Entry
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			 p_database_connection
	);
	Entry
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			p_database_connection,
		Id p_id
	);
	void set_journal_id(Journal::Id p_journal_id);
	void set_account(Account const& p_account);
	void set_comment(std::string const& p_comment);
	void set_amount(jewel::Decimal const& p_amount);
	void set_whether_reconciled(bool p_is_reconciled);
	std::string comment() const;
	jewel::Decimal amount() const;
	Account account() const;
	bool is_reconciled() const;

	/**
	 * TODO This should eventually be shifted into a base
	 * class.
	 */
	Id id() const;

	/**
	 * TODO This should eventually be shifted into a base
	 * class.
	 */
	void save();



private:
	sqloxx::Handle<EntryImpl> m_impl;

};


}  // namespace phatbooks


#endif  // GUARD_entry_hpp
