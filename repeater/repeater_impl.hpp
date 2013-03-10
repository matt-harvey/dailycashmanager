#ifndef GUARD_repeater_impl_hpp
#define GUARD_repeater_impl_hpp

/** \file repeater_impl.hpp
 *
 * \brief Header file pertaining to RepeaterImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "date.hpp"
#include "draft_journal.hpp"
#include "interval_type.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "proto_journal.hpp"
#include <sqloxx/persistent_object.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace phatbooks
{




/**
 * Provides implementation for Repeater. Multiple Repeater instances may
 * be associated with a single RepaeaterImpl (due to the shallow copy
 * semantics of Repeater).
 */
class RepeaterImpl:
	public sqloxx::PersistentObject<RepeaterImpl, PhatbooksDatabaseConnection>
{
public:
	
	typedef sqloxx::PersistentObject
	<	RepeaterImpl,
		PhatbooksDatabaseConnection
	>	PersistentObject;

	typedef typename PersistentObject::Id Id;	

	typedef sqloxx::IdentityMap<RepeaterImpl, PhatbooksDatabaseConnection>
		IdentityMap;

	typedef interval_type::IntervalType IntervalType;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit
	RepeaterImpl
	(	IdentityMap& p_identity_map
	);

	RepeaterImpl
	(	IdentityMap& p_identity_map,	
		Id p_id
	);

	~RepeaterImpl();

	void set_interval_type(IntervalType p_interval_type);

	void set_interval_units(int p_interval_units);

	void set_next_date(boost::gregorian::date const& p_next_date);

	void set_journal_id(DraftJournal::Id p_journal_id);
		
	IntervalType interval_type();

	int interval_units();

	/**
	 * @throws UnsafeArithmeticException in the extremely unlikely event of
	 * arithmetic overflow during execution.
	 *
	 * @todo Could it throw anything else?
	 */
	boost::gregorian::date next_date
	(	std::vector<boost::gregorian::date>::size_type n
	);

	/**
	 * @throws UnsafeArithmeticException in the extremely unlikely event of
	 * arithmetic overflow during calculation.
	 *
	 * @todo Could it throw anything else?
	 *
	 * @returns a smart pointer to a vector into which the
	 * list of firings occur in chronological order from
	 * soonest to latest.
	 */
	boost::shared_ptr<std::vector<boost::gregorian::date> >
	firings_till(boost::gregorian::date const& limit);

	/**
	 * @todo Determine exceptions and exception-safety. These are important
	 * here.
	 *
	 * @todo Testing (but see tests already done...)
	 */
	OrdinaryJournal fire_next();
	

	DraftJournal::Id journal_id();

	void swap(RepeaterImpl& rhs);

	static std::string primary_table_name();
	static std::string exclusive_table_name();
	static std::string primary_key_name();

private:

	RepeaterImpl(RepeaterImpl const& rhs);
	void do_load();
	void do_save_existing();
	void do_save_new();
	void do_ghostify();
	void process_saving_statement(sqloxx::SQLStatement& statement);

	struct RepeaterData;

	boost::scoped_ptr<RepeaterData> m_data;
};


struct RepeaterImpl::RepeaterData
{
	boost::optional<RepeaterImpl::IntervalType> interval_type;
	boost::optional<int> interval_units;
	boost::optional<DateRep> next_date;
	boost::optional<DraftJournal::Id> journal_id;
};





}  // namespace phatbooks

#endif  // GUARD_repeater_impl_hpp
