// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_entry_reader_hpp
#define GUARD_entry_reader_hpp

#include "entry.hpp"
#include "phatbooks_database_connection.hpp"
#include <sqloxx/reader.hpp>

namespace phatbooks
{

class Account;


typedef
	sqloxx::Reader<Entry, PhatbooksDatabaseConnection>
	EntryReader;

class ActualOrdinaryEntryReader: public EntryReader
{
public:
	explicit ActualOrdinaryEntryReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};



}  // namespace phatbooks


#endif  // GUARD_entry_reader_hpp
