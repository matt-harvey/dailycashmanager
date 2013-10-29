/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_proto_journal_hpp_2501613862114779
#define GUARD_proto_journal_hpp_2501613862114779

#include "journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "transaction_type.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/id.hpp>
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
