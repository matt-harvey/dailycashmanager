#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"

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
	/*
	db.store(Account(Account::balance_sheet, "Cash", "Notes and coins"));
	db.store(Account(Account::profit_and_loss, "Food", "Food and drink"));
	*/



	return 0;
}
