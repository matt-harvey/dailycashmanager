#include "account_storage_manager.hpp"
#include "account.hpp"
#include "general_typedefs.hpp"
#include "phatbooks_exceptions.hpp"
#include "sqloxx/database_connection.hpp"
#include "sqloxx/sql_statement.hpp"
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
	DatabaseConnection& dbc
)
{
	SQLStatement commodity_finder
	(	dbc,
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
	SQLStatement statement
	(	dbc,
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



}  // namespace sqloxx
