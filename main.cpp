#include "phatbooks_database_connection.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "entry.hpp"
#include "general_typedefs.hpp"
#include "journal.hpp"
#include "repeater.hpp"
#include "text_user_interface.hpp"

#include <iostream>

using phatbooks::Account;
using phatbooks::Commodity;
using phatbooks::PhatbooksDatabaseConnection;
using phatbooks::IdType;
using jewel::Decimal;
using std::cout;
using std::endl;
using phatbooks::text_user_interface::Menu;

void say_hello()
{
	return;
	cout << "Hello" << endl;
}

void say_goodbye()
{
	return;
	cout << "Goodbye" << endl;
}

void get_outta_here()
{
	return;
}

int main()
{
	Menu menu;
	menu.add_item("Say hello", say_hello);
	menu.add_item("Say goodbye", say_goodbye);
	menu.add_item("Quit", get_outta_here, "x");
	menu.present_to_user();




















	/*
	map<string, int> m;
	m["One"] = 1;
	m["Two"] = 2;

	map<string, int>::const_iterator it;
	if ((it = m.find("Two")) == m.end())
	{
		cout << "Couldn't find." << endl;
	}
	else
	{
		cout << it->second << endl;
	}
	*/


	/*

	PhatbooksDatabaseConnection db;
	db.open("/home/matthew/Workbench/versioned/phatbooks/test.db");	
	db.setup();

	Commodity dollars
	(	"AUD",
		"Australian dollars",
		"",
		2,
		Decimal("1")
	);
	IdType const dollars_id = db.store(dollars);
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
	IdType const cash_id = db.store(cash);
	IdType const food_id = db.store(food);

	cout << "Id for stored Commodity \"dollars\": " << dollars_id << endl;
	cout << "Id for stored Account \"cash\": " << cash_id << endl;
	cout << "Id for stored Account \"food\": " << food_id << endl;

	cout << "Cash account exists? " << db.has_account_named("Cash") << endl;
	cout << "Food account exists? " << db.has_account_named("Food") << endl;
	cout << "Entertainment account exists? " 
	     << db.has_account_named("Entertainment") << endl;
	
	*/

	return 0;
}
