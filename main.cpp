#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"

#include <iostream>

using phatbooks::Account;
using phatbooks::Commodity;
using phatbooks::PhatbooksDatabaseConnection;
using jewel::Decimal;
using std::cout;
using std::endl;

int main()
{

	PhatbooksDatabaseConnection db;
	db.open("/home/matthew/Workbench/versioned/phatbooks/test.db");	
	Commodity dollars
	(	"AUD",
		"Australian dollars",
		"",
		2,
		Decimal("1")
	);
	db.store(dollars);
	Account cash
	(	"Cash",
		"AUD",
		Account::balance_sheet,
		"Notes and coins"
	);
	Account food
	(	"Food",
		"AUD",
		Account::profit_and_loss,
		"Food and drink"
	);
	db.store(cash);
	db.store(food);

	return 0;
}
