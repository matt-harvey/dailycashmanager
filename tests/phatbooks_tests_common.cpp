// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "phatbooks_tests_common.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "visibility.hpp"
#include <boost/filesystem.hpp>
#include <jewel/decimal.hpp>
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <cstdlib>
#include <iostream>

using jewel::Decimal;
using sqloxx::Handle;
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
	Handle<Commodity> const australian_dollars(dbc);
	australian_dollars->set_abbreviation("AUD");
	australian_dollars->set_name("Australian dollars");
	australian_dollars->set_description("Australian currency in dollars");
	australian_dollars->set_precision(2);
	australian_dollars->set_multiplier_to_base(Decimal(1, 0));
	australian_dollars->save();
	
	Handle<Commodity> const us_dollars(dbc);
	us_dollars->set_abbreviation("USD");
	us_dollars->set_name("US dollars");
	us_dollars->set_description("United States currency in dollars");
	us_dollars->set_precision(2);
	us_dollars->set_multiplier_to_base(Decimal("0.95"));
	us_dollars->save();

	return;
}


void setup_test_accounts(PhatbooksDatabaseConnection& dbc)
{
	Handle<Account> const cash(dbc);
	cash->set_account_type(AccountType::asset);
	cash->set_name("cash");
	cash->set_commodity
	(	Handle<Commodity>(dbc, Commodity::id_for_abbreviation(dbc, "AUD"))
	);
	cash->set_description("notes and coins");
	cash->set_visibility(Visibility::visible);
	cash->save();

	Handle<Account> const food(dbc);
	food->set_account_type(AccountType::expense);
	food->set_name("food");
	food->set_commodity
	(	Handle<Commodity>(dbc, Commodity::id_for_abbreviation(dbc, "AUD"))
	);
	food->set_description("food and drink");
	food->set_visibility(Visibility::visible);
	food->save();

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
	setup_test_commodities(*pdbc);
	setup_test_accounts(*pdbc);
	JEWEL_ASSERT (pdbc->is_valid());
}


TestFixture::~TestFixture()
{
	JEWEL_ASSERT (pdbc->is_valid());
	delete pdbc;
	filesystem::remove(db_filepath);
	JEWEL_ASSERT (!file_exists(db_filepath));
}




}  // namespace test
}  // namespace phatbooks
