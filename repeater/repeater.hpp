#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

#include "interval_type.hpp"
#include "journal.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>

namespace phatbooks
{

class PhatbooksDatabaseConnection;
class RepeaterImpl;


class Repeater
{
public:
	typedef interval_type::IntervalType IntervalType;
	typedef sqloxx::Id Id;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	explicit Repeater
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	Repeater
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	void set_interval_type(IntervalType p_interval_type);
	void set_interval_units(int p_interval_units);
	void set_journal_id(Journal::Id p_journal_id);
	void set_next_date(boost::gregorian::date const& p_next_date);
	IntervalType interval_type() const;
	int interval_units() const;
	boost::gregorian::date next_date() const;
	Journal::Id journal_id() const;

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

	/**
	 * TODO This should eventually be shifted into a base class.
	 */
	void remove();

private:
	sqloxx::Handle<RepeaterImpl> m_impl;
};


}  // namespace phatbooks

#endif  // GUARD_repeater_hpp
