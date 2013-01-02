#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

#include "interval_type.hpp"
#include "draft_journal.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_persistent_object.hpp"
#include "proto_journal.hpp"
#include "repeater_impl.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace phatbooks
{

class Repeater:
	public PhatbooksPersistentObject<RepeaterImpl>
{
public:
	typedef
		PhatbooksPersistentObject<RepeaterImpl>
		PhatbooksPersistentObject;
	typedef PhatbooksPersistentObjectBase::Id Id;

	typedef interval_type::IntervalType IntervalType;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);
	
	explicit Repeater
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	Repeater
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	static Repeater create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);


	void set_interval_type(IntervalType p_interval_type);

	void set_interval_units(int p_interval_units);

	// This should not normally be set by client code as the
	// DraftJournal::add_repeater(...) and save() methods take
	// care of assigning the correct journal_id to the
	// repeater.
	void set_journal_id(ProtoJournal::Id p_journal_id);

	void set_next_date(boost::gregorian::date const& p_next_date);

	boost::shared_ptr<std::vector<boost::gregorian::date> >
	firings_till(boost::gregorian::date const& limit);

	OrdinaryJournal fire_next();

	IntervalType interval_type() const;

	int interval_units() const;

	boost::gregorian::date next_date
	(	std::vector<boost::gregorian::date>::size_type n = 0
	) const;

	DraftJournal::Id journal_id() const;

private:
	Repeater(sqloxx::Handle<RepeaterImpl> const& p_handle);
};



/**
 * @returns an English phrase describing the frequency of the
 * Repeater.
 */
std::string frequency_description(Repeater const& repeater);


}  // namespace phatbooks

#endif  // GUARD_repeater_hpp
