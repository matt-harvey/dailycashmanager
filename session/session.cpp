// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "phatbooks_database_connection.hpp"
#include "entry_reader.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "repeater_reader.hpp"
#include "session.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <cassert>
#include <list>
#include <string>

#ifdef DEBUG
#	include <jewel/debug_log.hpp>
#	include <iostream>
#endif

using boost::shared_ptr;
using sqloxx::InvalidFilename;
using std::list;
using std::string;
namespace gregorian = boost::gregorian;

namespace phatbooks
{

int Session::s_num_instances = 0;

// TODO This is ignoring the PhatbooksDatabaseConnection which we
// have ALREADY created as part of the Session constructor.
// The whole Session hierarchy, and the interaction between
// GraphicalSession and App, is a bit of a mess.
Session::Session():
	m_database_connection(new PhatbooksDatabaseConnection)
{
	database_connection().set_caching_level(s_default_caching_level);
	++s_num_instances;
	if (s_num_instances > s_max_instances)
	{
		--s_num_instances;
		throw TooManySessions
		(	"Exceeded maximum number of instances of phatbooks::Session."
		);
	}
	assert (s_num_instances <= s_max_instances);
}


Session::~Session()
{
	assert (s_num_instances > 0);
	assert (s_num_instances <= s_max_instances);
	--s_num_instances;
}


int
Session::run()
{
	return do_run();
}


int
Session::run(string const& filepath_str)
{
	if (filepath_str.empty())
	{
		throw InvalidFilename("Filename is empty string.");
	}	
	return do_run(filepath_str);
}


PhatbooksDatabaseConnection&
Session::database_connection() const
{
	return *m_database_connection;
}






}  // namespace phatbooks
