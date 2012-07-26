#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"

/*
#include <Poco/Data/Common.h>
#include <Poco/Data/SQLite/Connector.h>
*/


// #include "UnitTest++/src/UnitTest++.h"

#include <iostream>

using phatbooks::Account;
using phatbooks::PhatbooksDatabaseConnection;
using jewel::Decimal;
using std::cout;
using std::endl;

// For now while I play with Poco...
// using namespace Poco::Data;


/*
void init()
{
	SQLite::Connector::registerConnector();
}

void shutdown()
{
	SQLite::Connector::unregisterConnector();
}
*/


int main()
{
	/*
	init();
	Session ses("SQLite", "sample.db");
	int count = 0;
	ses << "create table tab(num integer)", now;
	shutdown();
	*/



	PhatbooksDatabaseConnection db;
	db.open("/home/matthew/Workbench/versioned/phatbooks/test.db");	
	db.store(Account(Account::balance_sheet, "Cash", "Notes and coins"));
	db.store(Account(Account::profit_and_loss, "Food", "Food and drink"));
	
	return 0;
}
