// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "date.hpp"
#include "interval_type.hpp"
#include "phatbooks_tests_common.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <UnitTest++/UnitTest++.h>
#include <cassert>


namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace test
{


TEST(test_is_valid_date_for_interval_type)
{
	for (int y = 1990; y <= 2015; y += 3)
	{
		for (int m = 1; m != 13; ++m)
		{
			gregorian::date const first(y, m, 1);
			gregorian::date const last =
				first - gregorian::date_duration(1);
			assert (last.day() >= 28);
			CHECK
			(	is_valid_date_for_interval_type
				(	last,
					interval_type::month_ends
				)
			);
			for
			(	gregorian::date prev = last;
				prev.day() != 28;
				prev -= gregorian::date_duration(1)
			)
			{
				assert (prev.day() >= 28);
				assert (prev.day() <= 31);
				CHECK
				(	!is_valid_date_for_interval_type
					(	prev,
						interval_type::months
					)
				);
			}
			for (int d = 1; d != 29; ++d)
			{
				gregorian::date dt(y, m, d);
				CHECK
				(	is_valid_date_for_interval_type
					(	dt,
						interval_type::days
					)
				);
				CHECK
				(	is_valid_date_for_interval_type
					(	dt,
						interval_type::weeks
					)
				);
				CHECK
				(	is_valid_date_for_interval_type
					(	dt,
						interval_type::months
					)
				);
			}
		}
	}
}


}  // namespace test
}  // namespace phatbooks
