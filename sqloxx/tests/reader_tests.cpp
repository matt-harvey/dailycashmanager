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

// TODO Uncomment and get these tests to work

/*
// Our base Reader class for reading DerivedPO
typedef
	Reader<DerivedPO, DerivedDatabaseConnection>
	DerivedPOReader;

// A derived Reader class
class FiveReader:
	public DerivedPOReader
{
public:
	FiveReader(DerivedDatabaseConnection& p_database_connection):
		DerivedPOReader
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
	DerivedPOReader reader1(*pdbc);
	int i = 0;
	while (reader1.read()) ++i;
	CHECK_EQUAL(i, 5);
	DerivedPOReader reader2(*pdbc);
	int max = 0;
	while (reader2.read())
	{
		max =
		(	reader2.item()->x() > max?
			reader2.item()->x():
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
	int i = 0;
	while (reader1.read()) ++i;
	CHECK_EQUAL(i, 2);
}


TEST_FIXTURE
(	DerivedPOFixture,
	test_reader_constructor_and_basic_functioning_3
)
{
	setup_reader_test(*pdbc);
	DerivedPOReader reader1
	(	*pdbc,
		"select derived_po_id from derived_pos where y > 14.2"
	);
	int i = 0;
	while (reader1.read()) ++i;
	CHECK_EQUAL(i, 3);
}


TEST_FIXTURE(DerivedPOFixture, test_reader_constructor_exceptions)
{
	DerivedDatabaseConnection invalid_dbc;
	CHECK_THROW(DerivedPOReader reader1(invalid_dbc), InvalidConnection);
	CHECK_THROW(FiveReader reader2(invalid_dbc), InvalidConnection);

	setup_reader_test(*pdbc);

	CHECK_THROW
	(	DerivedPOReader reader3
		(	*pdbc,
			"qselect unsyntactical gobbledigook from jbooble"
		),
		SQLiteException
	);
	CHECK_THROW
	(	DerivedPOReader reader4
		(	*pdbc,
			"select nonexistent_column from derived_pos"
		),
		SQLiteException
	);
	CHECK_THROW
	(	DerivedPOReader reader5
		(	*pdbc,
			"select derived_po_id from nonexistent_table"
		),
		SQLiteException
	);
	CHECK_THROW
	(	DerivedPOReader reader6
		(	*pdbc,
			"select derived_po_id from derived_pos; "
			"select derived_po_id_from derived_pos where x = 5"
		),
		TooManyStatements
	);
	CHECK_THROW
	(	DerivedPOReader reader7
		(	*pdbc,
			"select derived_po_id from derived_pos; um"
		),
		TooManyStatements
	);
}


TEST_FIXTURE(DerivedPOFixture, test_reader_read)
{
	// Basic functioning
	setup_reader_test(*pdbc);
	DerivedPOReader reader1(*pdbc);

	int i = 0;
	while (reader1.read()) ++i;
	CHECK_EQUAL(i, 5);
	
	// If we keep reading it goes back to the beginning and just
	// keeps cycling round.
	CHECK(reader1.read());
	CHECK(reader1.read());
	CHECK(reader1.read());
	CHECK(reader1.read());
	CHECK(reader1.read());
	CHECK(!reader1.read()); // At end
	CHECK(reader1.read());  // Back at beginning
	CHECK(reader1.read());
}

TEST_FIXTURE(DerivedPOFixture, test_reader_item)
{
	setup_reader_test(*pdbc);
	DerivedPOReader reader1
	(	*pdbc,
		"select derived_po_id from derived_pos order by derived_po_id"
	);
	for (int i = 1; reader1.read(); ++i)
	{
		CHECK_EQUAL(reader1.item()->id(), i);
	}
	CHECK_THROW(reader1.item(), InvalidReader);

	DerivedPOReader reader2(*pdbc);
	while (reader2.read())
	{
		Handle<DerivedPO> dpo2(reader2.item());
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
	
	CHECK_THROW(reader2.item(), InvalidReader);
}
*/


}  // namespace tests
}  // namespace sqloxx
