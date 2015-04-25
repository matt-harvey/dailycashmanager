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
#include <boost/test/unit_test.hpp>
#include <sqloxx/id.hpp>
#include <utility>

using sqloxx::Id;
using std::move;

namespace gregorian = boost::gregorian;

namespace dcm
{
namespace test
{

BOOST_AUTO_TEST_CASE(test_repeater_firing_result_constructors_and_getters)
{
    RepeaterFiringResult rfs0(1, gregorian::date(2018, 9, 20), true);
    RepeaterFiringResult const rfs1(239, gregorian::date(1999, 11, 3), false);
    RepeaterFiringResult rfs2(29, gregorian::date(3000, 2, 28), false);

    BOOST_CHECK_EQUAL(rfs0.draft_journal_id(), 1);
    BOOST_CHECK_EQUAL(rfs0.firing_date(), gregorian::date(2018, 9, 20));
    BOOST_CHECK_EQUAL(rfs0.successful(), true);
    BOOST_CHECK_EQUAL(rfs1.successful(), false);
    BOOST_CHECK_EQUAL(rfs1.draft_journal_id(), 239);
    BOOST_CHECK_EQUAL(rfs1.firing_date(), gregorian::date(1999, 11, 3));
    BOOST_CHECK_EQUAL(rfs2.firing_date(), gregorian::date(3000, 2, 28));
    BOOST_CHECK_EQUAL(rfs2.successful(), false);
    BOOST_CHECK_EQUAL(rfs2.draft_journal_id(), 29);

    RepeaterFiringResult const rfs0b(rfs0);
    RepeaterFiringResult const rfs0c = rfs0;
    BOOST_CHECK_EQUAL(rfs0b.draft_journal_id(), 1);
    BOOST_CHECK_EQUAL(rfs0b.firing_date(), gregorian::date(2018, 9, 20));
    BOOST_CHECK_EQUAL(rfs0b.successful(), true);
    BOOST_CHECK_EQUAL(rfs0c.draft_journal_id(), 1);
    BOOST_CHECK_EQUAL(rfs0c.firing_date(), gregorian::date(2018, 9, 20));
    BOOST_CHECK_EQUAL(rfs0c.successful(), true);

    RepeaterFiringResult const rfs1b = std::move(rfs1);
    // Don't use rfs1 after this point!
    BOOST_CHECK_EQUAL(rfs1b.successful(), false);
    BOOST_CHECK_EQUAL(rfs1b.draft_journal_id(), 239);
    BOOST_CHECK_EQUAL(rfs1b.firing_date(), gregorian::date(1999, 11, 3));
}

BOOST_AUTO_TEST_CASE(test_repeater_firing_result_assignment_and_getters)
{
    RepeaterFiringResult const rfs0(89, gregorian::date(2018, 9, 20), true);
    RepeaterFiringResult rfs1(239, gregorian::date(1999, 11, 3), false);

    BOOST_CHECK_EQUAL(rfs0.draft_journal_id(), 89);
    BOOST_CHECK_EQUAL(rfs0.firing_date(), gregorian::date(2018, 9, 20));
    BOOST_CHECK_EQUAL(rfs0.successful(), true);
    BOOST_CHECK_EQUAL(rfs1.draft_journal_id(), 239);
    BOOST_CHECK_EQUAL(rfs1.successful(), false);
    BOOST_CHECK_EQUAL(rfs1.firing_date(), gregorian::date(1999, 11, 3));
    
    rfs1 = rfs0;

    BOOST_CHECK_EQUAL(rfs1.draft_journal_id(), 89);
    BOOST_CHECK_EQUAL(rfs1.draft_journal_id(), rfs0.draft_journal_id());
    BOOST_CHECK_EQUAL(rfs1.firing_date(), gregorian::date(2018, 9, 20));
    BOOST_CHECK_EQUAL(rfs1.firing_date(), rfs0.firing_date());
    BOOST_CHECK_EQUAL(rfs1.successful(), true);
    BOOST_CHECK_EQUAL(rfs1.successful(), rfs0.successful());

    rfs1 = rfs1;

    BOOST_CHECK_EQUAL(rfs1.draft_journal_id(), 89);
    BOOST_CHECK_EQUAL(rfs1.draft_journal_id(), rfs0.draft_journal_id());
    BOOST_CHECK_EQUAL(rfs1.firing_date(), gregorian::date(2018, 9, 20));
    BOOST_CHECK_EQUAL(rfs1.firing_date(), rfs0.firing_date());
    BOOST_CHECK_EQUAL(rfs1.successful(), true);
    BOOST_CHECK_EQUAL(rfs1.successful(), rfs0.successful());
}

BOOST_AUTO_TEST_CASE(test_repeater_firing_result_mark_as_successful)
{
    RepeaterFiringResult rfs2(29, gregorian::date(3000, 2, 28), false);
    BOOST_CHECK_EQUAL(rfs2.successful(), false);
    rfs2.mark_as_successful();
    BOOST_CHECK_EQUAL(rfs2.successful(), true);
}

BOOST_AUTO_TEST_CASE(test_repeater_firing_result_operator_less_than)
{
    RepeaterFiringResult const rfs0(1, gregorian::date(2018, 9, 20), true);
    RepeaterFiringResult const rfs1(239, gregorian::date(1999, 11, 3), false);
    RepeaterFiringResult const rfs2(29, gregorian::date(3000, 2, 28), false);
    RepeaterFiringResult rfs3(70, gregorian::date(3000, 2, 28), true);

    BOOST_CHECK(rfs1 < rfs0);
    BOOST_CHECK(rfs1 < rfs2);
    BOOST_CHECK(rfs0 < rfs2);
    BOOST_CHECK(!(rfs0 < rfs1));
    BOOST_CHECK(!(rfs2 < rfs1));
    BOOST_CHECK(!(rfs2 < rfs0));
    BOOST_CHECK(!(rfs0 < rfs0));
    BOOST_CHECK(!(rfs1 < rfs1));
    BOOST_CHECK(!(rfs2 < rfs2));
    BOOST_CHECK(!(rfs3 < rfs2));
    BOOST_CHECK(!(rfs2 < rfs3));
}

}  // namespace test
}  // namespace gregorian
