#include "derived_po.hpp"
#include "sqloxx_tests_common.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/handle.hpp"
#include "sqloxx/reader.hpp"
#include "sqloxx/sqloxx_exceptions.hpp"
#include <unittest++/UnitTest++.h>

#include <iostream>  // For logging
using std::cout;
using std::endl;

namespace sqloxx
{
namespace tests
{


// Our base Reader class for reading DerivedPO
typedef
	Reader< Handle<DerivedPO> , DerivedDatabaseConnection >
	DerivedPOHandleReader;

// A derived Reader class
class FiveReader:
	public DerivedPOHandleReader
{
public:
	FiveReader(DerivedDatabaseConnection& p_database_connection):
		DerivedPOHandleReader
		(	p_database_connection,
			"select derived_po_id from derived_pos where x = 5"
		)
	{
	}

};

// Setup
void setup_reader_test(DerivedDatabaseConnection& dbc)
{
	Handle<DerivedPO> dpo1(dbc);
	Handle<DerivedPO> dpo2(dbc);
	Handle<DerivedPO> dpo3(dbc);
	Handle<DerivedPO> dpo4(dbc);
	Handle<DerivedPO> dpo5(dbc);
	dpo1->set_x(0);
	dpo1->set_y(14.1);
	dpo2->set_x(5);
	dpo2->set_y(14.2);
	dpo3->set_x(10);
	dpo3->set_y(14.3);
	dpo4->set_x(0);
	dpo4->set_y(14.4);
	dpo5->set_x(5);
	dpo5->set_y(14.5);
	dpo1->save();
	dpo2->save();
	dpo3->save();
	dpo4->save();
	dpo5->save();
}

TEST_FIXTURE
(	DerivedPOFixture,
	test_reader_constructor_and_basic_functioning_1
)
{
	setup_reader_test(*pdbc);
	DerivedPOHandleReader reader1(*pdbc);
	CHECK_EQUAL(reader1.size(), 5);
	int i = 0;
	for
	(	DerivedPOHandleReader::const_iterator it = reader1.begin();
		it != reader1.end();
		++it, ++i
	)
	{
	}
	CHECK_EQUAL(i, 5);
	DerivedPOHandleReader reader2(*pdbc);
	int max = 0;
	for
	(	DerivedPOHandleReader::iterator it = reader2.begin();
		it != reader2.end();
		++it
	)
	{
		max =
		(	(*it)->x() > max?
			(*it)->x():
			max
		);
	}
	CHECK_EQUAL(max, 10);
}
TEST_FIXTURE
(	DerivedPOFixture,
	test_reader_constructor_and_basic_functioning_2
)
{
	setup_reader_test(*pdbc);
	FiveReader reader1(*pdbc);
	CHECK_EQUAL(reader1.size(), 2);
}

TEST_FIXTURE
(	DerivedPOFixture,
	test_reader_constructor_and_basic_functioning_3
)
{
	setup_reader_test(*pdbc);
	DerivedPOHandleReader reader1
	(	*pdbc,
		"select derived_po_id from derived_pos where y > 14.2"
	);
	CHECK_EQUAL(reader1.size(), 3);
}




TEST_FIXTURE(DerivedPOFixture, test_reader_constructor_exceptions)
{
	DerivedDatabaseConnection invalid_dbc;
	CHECK_THROW(DerivedPOHandleReader reader1(invalid_dbc), InvalidConnection);
	CHECK_THROW(FiveReader reader2(invalid_dbc), InvalidConnection);

	setup_reader_test(*pdbc);

	CHECK_THROW
	(	DerivedPOHandleReader reader3
		(	*pdbc,
			"qselect unsyntactical gobbledigook from jbooble"
		),
		SQLiteException
	);
	CHECK_THROW
	(	DerivedPOHandleReader reader4
		(	*pdbc,
			"select nonexistent_column from derived_pos"
		),
		SQLiteException
	);
	CHECK_THROW
	(	DerivedPOHandleReader reader5
		(	*pdbc,
			"select derived_po_id from nonexistent_table"
		),
		SQLiteException
	);
	CHECK_THROW
	(	DerivedPOHandleReader reader6
		(	*pdbc,
			"select derived_po_id from derived_pos; "
			"select derived_po_id_from derived_pos where x = 5"
		),
		TooManyStatements
	);
	CHECK_THROW
	(	DerivedPOHandleReader reader7
		(	*pdbc,
			"select derived_po_id from derived_pos; um"
		),
		TooManyStatements
	);
}


TEST_FIXTURE(DerivedPOFixture, test_reader_deref)
{
	setup_reader_test(*pdbc);
	DerivedPOHandleReader reader1
	(	*pdbc,
		"select derived_po_id from derived_pos order by derived_po_id"
	);
	int i = 1;
	for
	(	DerivedPOHandleReader::iterator it = reader1.begin();
		it != reader1.end();
		++it, ++i	
	)
	{
		CHECK_EQUAL((*it)->id(), i);
	}

	DerivedPOHandleReader reader2(*pdbc);
	for
	(	DerivedPOHandleReader::iterator it = reader2.begin();
		it != reader2.end();
		++it
	)
	{
		Handle<DerivedPO> dpo2 = *it;  // TODO Why can't I use copy constructor here?
		int const id = dpo2->id();
		switch (id)
		{
		case 1:
			CHECK_EQUAL(dpo2->x(), 0);
			CHECK_EQUAL(dpo2->y(), 14.1);
			break;
		case 2:
			CHECK_EQUAL(dpo2->x(), 5);
			CHECK_EQUAL(dpo2->y(), 14.2);
			break;
		case 3:
			CHECK_EQUAL(dpo2->x(), 10);
			CHECK_EQUAL(dpo2->y(), 14.3);
			break;
		case 4:
			CHECK_EQUAL(dpo2->x(), 0);
			CHECK_EQUAL(dpo2->y(), 14.4);
			break;
		case 5:
			CHECK_EQUAL(dpo2->x(), 5);
			CHECK_EQUAL(dpo2->y(), 14.5);
			break;
		default:
			CHECK(false);  // Execution should never reach here.
			break;
		}
	}
}


}  // namespace tests
}  // namespace sqloxx

