// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_proto_journal_hpp_2501613862114779
#define GUARD_proto_journal_hpp_2501613862114779

/** \file proto_journal.hpp
 *
 * \brief Header file relating to ProtoJournal class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <wx/string.h>
#include <memory>
#include <ostream>
#include <string>
#include <vector>


namespace phatbooks
{


/**
 * Class representing a Journal that has not yet taken on either
 * OrdinaryJournal or DraftJournal characteristics. This will generally
 * be a Journal that is in the process of being "constructed" by the
 * user.
 */
class ProtoJournal: public Journal
{
public:
	ProtoJournal() = default;
	ProtoJournal(ProtoJournal&&) = default;
	ProtoJournal& operator=(ProtoJournal const&) = default;
	ProtoJournal& operator=(ProtoJournal&&) = delete;
	virtual ~ProtoJournal();

protected:
	ProtoJournal(ProtoJournal const& rhs) = default;

};


// IMPLEMENT INLINE FUNCTIONS

inline
ProtoJournal::~ProtoJournal()
{
}

}  // namespace phatbooks

#endif  // GUARD_proto_journal_hpp_2501613862114779
