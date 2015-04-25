/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_proto_journal_hpp_2501613862114779
#define GUARD_proto_journal_hpp_2501613862114779

#include "journal.hpp"
#include "dcm_database_connection.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/id.hpp>
#include <wx/string.h>
#include <memory>
#include <ostream>
#include <string>
#include <vector>


namespace dcm
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

}  // namespace dcm

#endif  // GUARD_proto_journal_hpp_2501613862114779
