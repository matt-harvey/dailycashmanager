#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"

// #include "UnitTest++/src/UnitTest++.h"

#include <iostream>

using phatbooks::Account;
using phatbooks::PhatbooksDatabaseConnection;
using jewel::Decimal;
using std::cout;
using std::endl;

int main()
{
	PhatbooksDatabaseConnection db;
	db.open("/home/matthew/Workbench/versioned/phatbooks/test.db");	
	Account acc(Account::balance_sheet, "Cash", "Notes and coins");
	db.store(acc);
	return 0;
}
