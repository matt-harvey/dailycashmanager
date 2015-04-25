/*
 * Copyright 2014 Matthew Harvey
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

#include "repeater_firing_result.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <sqloxx/id.hpp>

using sqloxx::Id;

namespace gregorian = boost::gregorian;

namespace dcm
{

RepeaterFiringResult::RepeaterFiringResult
(   Id p_draft_journal_id,
    gregorian::date const& p_firing_date,
    bool p_successful
):
    m_draft_journal_id(p_draft_journal_id),
    m_firing_date(p_firing_date),
    m_successful(p_successful)
{
}

Id
RepeaterFiringResult::draft_journal_id() const
{
    return m_draft_journal_id;
}

gregorian::date
RepeaterFiringResult::firing_date() const
{
    return m_firing_date;
}

bool
RepeaterFiringResult::successful() const
{
    return m_successful;
}

void
RepeaterFiringResult::mark_as_successful()
{
    m_successful = true;
    return;
}

bool
operator<(RepeaterFiringResult const& lhs, RepeaterFiringResult const& rhs)
{
    return lhs.firing_date() < rhs.firing_date();
}

}  // namespace dcm
