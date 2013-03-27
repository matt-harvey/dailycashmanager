#include "phatbooks_tests_common.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <jewel/decimal.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>

using jewel::Decimal;
using std::abort;
using std::cout;
using std::cerr;
using std::endl;

namespace filesystem = boost::filesystem;


namespace phatbooks
{
namespace test
{


bool file_exists(filesystem::path const& filepath)
{
	return filesystem::exists
	(	filesystem::status(filepath)
	);
}


void abort_if_exists(filesystem::path const& filepath)
{
	if (file_exists(filepath))
	{
		cerr << "File named \"" << filepath.string() << "\" already "
			 << "exists. Test aborted." << endl;
		abort();
	}
	return;
}


void setup_test_commodities(PhatbooksDatabaseConnection& dbc)
{
	Commodity australian_dollars(dbc);
	australian_dollars.set_abbreviation("AUD");
	australian_dollars.set_name("Australian dollars");
	australian_dollars.set_description("Australian currency in dollars");
	australian_dollars.set_precision(2);
	australian_dollars.set_multiplier_to_base(Decimal(1, 0));
	australian_dollars.save();
	
	Commodity us_dollars(dbc);
	us_dollars.set_abbreviation("USD");
	us_dollars.set_name("US dollars");
	us_dollars.set_description("United States currency in dollars");
	us_dollars.set_precision(2);
	us_dollars.set_multiplier_to_base(Decimal("0.95"));
	us_dollars.save();

	return;
}


void setup_test_accounts(PhatbooksDatabaseConnection& dbc)
{
	Account cash(dbc);
	cash.set_account_type(account_type::asset);
	cash.set_name("cash");
	cash.set_commodity(Commodity(dbc, "AUD"));
	cash.set_description("notes and coins");
	cash.set_opening_balance(Decimal("0.00"));
	cash.save();

	Account food(dbc);
	food.set_account_type(account_type::expense);
	food.set_name("food");
	food.set_commodity(Commodity(dbc, "AUD"));
	food.set_description("food and drink");
	food.set_opening_balance(Decimal("0.00"));
	food.save();

	return;
}


TestFixture::TestFixture():
	db_filepath("Testfile_827787293.db"),
	pdbc(0)
{
	pdbc = new PhatbooksDatabaseConnection;
	abort_if_exists(db_filepath);
	pdbc->open(db_filepath);
	pdbc->set_caching_level(10);
	pdbc->setup();
	setup_test_commodities(*pdbc);
	setup_test_accounts(*pdbc);
	assert (pdbc->is_valid());
}


TestFixture::~TestFixture()
{
	assert (pdbc->is_valid());
	delete pdbc;
	filesystem::remove(db_filepath);
	assert (!file_exists(db_filepath));
}




}  // namespace test
}  // namespace phatbooks
