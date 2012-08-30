#include "account_storage_manager.hpp"
#include "general_typedefs.hpp"
#include "phatbooks_exceptions.hpp"
#include <string>
#include <vector>

using phatbooks::Account;
using phatbooks::IdType;
using phatbooks::PhatbooksException;
using phatbooks::StoragePreconditionsException;
using std::string;
using std::vector;

namespace sqloxx
{


void StorageManager<Account>::save
(	Account const& account,
	DatabaseConnection& db
)
{
	DatabaseConnection::SQLStatement commodity_finder
	(	db,
		"select commodity_id from commodities where "
		"commodities.abbreviation = :p"
	);
	commodity_finder.bind(":p", account.commodity_abbreviation());
	if (!commodity_finder.step())
	{
		throw StoragePreconditionsException
		(	"Attempted to store Account with invalid "
			"commodity abbreviation."
		);
	}
	IdType commodity_id =
		commodity_finder.extract<IdType>(0);
	if (commodity_finder.step())
	{
		throw PhatbooksException
		(	"Integrity of commodities table has been violated. Table "
			"contains multiple rows with the same commodity abbreviation."
		);
	}
	DatabaseConnection::SQLStatement statement
	(	db,
		"insert into accounts(account_type_id, name, description, "
		"commodity_id) values(:account_type_id, :name, :description, "
		":commodity_id)"
	);
	statement.bind
	(	":account_type_id",
		static_cast<int>(account.account_type())
	);
	statement.bind(":name", account.name());
	statement.bind(":description", account.description());
	statement.bind(":commodity_id", commodity_id);
	statement.quick_step();
	return;
}



void StorageManager<Account>::setup_tables
(	DatabaseConnection& dbc
)
{
	typedef DatabaseConnection::SQLStatement Statement;
	Statement account_types_table_stmt
	(	dbc,
		"create table account_types(account_type_id integer primary key "
		"autoincrement, name text not null unique)"
	);
	account_types_table_stmt.quick_step();

	// Values inserted into account_types here must correspond with
	// AccountType enum defined in Account class
	vector<string> const names =
		Account::account_type_names();
	for (vector<string>::size_type i = 0; i != names.size(); ++i)
	{
		string const str =
			"insert into account_types(name) values('" + names[i] + "')";
		Statement populator(dbc, str);
		populator.quick_step();
	}
	Statement accounts_table_stmt
	(	dbc,
		"create table accounts "
		"("
			"account_id integer primary key autoincrement, "
			"account_type_id not null references account_types, "
			"name text not null unique, "
			"description text, "
			"commodity_id references commodities"
		")"
	);
	accounts_table_stmt.quick_step();
	Statement account_view_stmt
	( 	dbc,
		"create view accounts_extended as "
		"select account_id, account_type_id, accounts.name, "
		"accounts.description, commodities.abbreviation, "
		"commodity_id, "
		"commodities.multiplier_to_base_intval, "
		"commodities.multiplier_to_base_places from "
		"accounts join commodities using(commodity_id)"
	);
	account_view_stmt.quick_step();
	return;
}


Account StorageManager<Account>::load
(	StorageManager<Account>::Key const& name,
	DatabaseConnection& dbc
)
{
	DatabaseConnection::SQLStatement statement
	(	dbc,
		"select accounts.name, "
		"commodities.abbreviation, "
		"accounts.account_type_id, "
		"accounts.description from "
		"accounts_extended where accounts.name = :p"
	);
	statement.bind(":p", name);
	statement.step();
	return Account
	(	statement.extract<string>(0),
		statement.extract<string>(1),
		static_cast<Account::AccountType>(statement.extract<int>(2)),
		statement.extract<string>(3)
	);
}

}  // namespace sqloxx
