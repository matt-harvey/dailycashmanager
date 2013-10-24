/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "budget_item.hpp"
#include "date.hpp"
#include "string_conv.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "transaction_type.hpp"
#include "visibility.hpp"
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/string.h>
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

using boost::numeric_cast;
using boost::optional;
using jewel::clear;
using jewel::Decimal;
using jewel::Log;
using jewel::value;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::SQLStatement;
using std::find_if;
using std::map;
using std::string;
using std::vector;

namespace gregorian = boost::gregorian;


namespace phatbooks
{

namespace
{
	// Convert a "technical balance" to a "friendly balance",
	// where the balance is the balance of an Account with
	// account_super_type() ast.
	Decimal technical_to_friendly
	(	Decimal const& d,
		AccountSuperType ast
	)
	{
		switch (ast)
		{
		case AccountSuperType::balance_sheet:
			return d;
		case AccountSuperType::pl:
			return round(d * Decimal(-1, 0), d.places());
		default:
			JEWEL_HARD_ASSERT (false);
		}
	}

typedef
	PhatbooksDatabaseConnection::BalanceCacheAttorney
	BalanceCacheAttorney;

typedef
	PhatbooksDatabaseConnection::BudgetAttorney
	BudgetAttorney;

}  // end anonymous namespace

struct Account::AccountData
{
	optional<wxString> name;
	optional<Handle<Commodity> > commodity;
	optional<AccountType> account_type;
	optional<wxString> description;
	optional<Visibility> visibility;
};

void
Account::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table visibilities"
		"("
			"visibility_id integer primary key"
		");"
	);
	for
	(	int i = 0;
		i != static_cast<int>(Visibility::num_visibilities);
		++i
	)
	{
		SQLStatement statement
		(	dbc,
			"insert into visibilities(visibility_id) values(:p)"
		);
		statement.bind(":p", i);
		statement.step_final();
	}
	dbc.execute_sql
	(	"create table account_types(account_type_id integer primary key)"
	);
	vector<wxString>::size_type const num_account_types =
		account_type_names().size();
	for (vector<wxString>::size_type i = 1; i <= num_account_types; ++i)
	{
		SQLStatement statement
		(	dbc,	
			"insert into account_types(account_type_id) values(:p)"
		);
		statement.bind(":p", numeric_cast<Id>(i));
		statement.step_final();
	}

#	ifndef DEBUG
		SQLStatement checker
		(	dbc,
			"select max(account_type_id) from account_types"
		);
		checker.step();
		Id const maxi = checker.extract<Id>(0);
		JEWEL_ASSERT (maxi == 6);
		checker.step_final();
#	endif

	dbc.execute_sql
	(	"create table accounts "
		"("
			"account_id integer primary key autoincrement, "
			"account_type_id not null references account_types, "
			"name text not null unique, "
			"description text, "
			"commodity_id references commodities, "
			"visibility_id references visibilities"
		"); "
	);

	return;
}

sqloxx::Id
Account::id_for_name
(	PhatbooksDatabaseConnection& dbc,
	wxString const& name
)
{
	// This is a crude linear search, but should be fast enough
	// unless the user has a truly huge number of Accounts.
	// We do it this way as we need to match case-insensitively,
	// and in a way that respects locale. So we do the string matching
	// in the application code rather than in SQL.
	wxString const target = name.Lower();
	SQLStatement statement
	(	dbc,
		"select account_id, name from accounts"
	);
	while (statement.step())
	{
		wxString const candidate =
			std8_to_wx(statement.extract<string>(1)).Lower();
		if (candidate == target)
		{
			return statement.extract<sqloxx::Id>(0);
		}
	}
	JEWEL_THROW
	(	InvalidAccountNameException,
		"There is no Account with the passed name."
	);
}

Account::Account
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map),
	m_data(new AccountData)
{
	(void)p_signature;  // silence compiler re. unused param.
}

Account::Account
(	IdentityMap& p_identity_map,	
	Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map, p_id),
	m_data(new AccountData)
{
	(void)p_signature;  // silence compiler re. unused parameter
}

Account::Account(Account const& rhs):
	PersistentObject(rhs),
	m_data(new AccountData(*(rhs.m_data)))
{
}

Account::~Account() = default;

bool
Account::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
)
{
	wxString const target = p_name.Lower();
	SQLStatement statement(p_database_connection, "select name from accounts");
	while (statement.step())
	{
		wxString const candidate =
			std8_to_wx(statement.extract<string>(0)).Lower();
		if (candidate == target)
		{
			return true;
		}
	}
	return false;
}

bool
Account::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	sqloxx::Id p_id
)
{
	return PersistentObject::exists(p_database_connection, p_id);
}

bool
Account::no_user_pl_accounts_saved
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	SQLStatement statement
	(	p_database_connection,
		"select account_id, account_type_id from accounts"
	);
	Handle<Account> const balancing_account =
		p_database_connection.balancing_account();
	while (statement.step())
	{
		AccountType const atype =
			static_cast<AccountType>(statement.extract<int>(1));
		if (super_type(atype) == AccountSuperType::pl)
		{
			Handle<Account> const account
			(	p_database_connection,
				statement.extract<Id>(0)
			);
			if (account != balancing_account)
			{
				return false;
			}
		}
	}
	return true;
}

bool
Account::none_saved_with_account_type
(	PhatbooksDatabaseConnection& p_database_connection,
	AccountType p_account_type
)
{
	SQLStatement statement
	(	p_database_connection,
		"select account_type_id from accounts where "
		"account_type_id = :p"
	);
	statement.bind(":p", static_cast<int>(p_account_type));
	return !statement.step();
}

bool
Account::none_saved_with_account_super_type
(	PhatbooksDatabaseConnection& p_database_connection,
	AccountSuperType p_account_super_type
)
{
	SQLStatement statement
	(	p_database_connection,
		"select account_type_id from accounts"
	);
	while (statement.step())
	{
		AccountType const atype =
			static_cast<AccountType>(statement.extract<int>(0));
		if (super_type(atype) == p_account_super_type)
		{
			return false;
		}
	}
	return true;
}

AccountType
Account::account_type()
{
	load();
	return value(m_data->account_type);
}

AccountSuperType
Account::account_super_type()
{
	load();
	return super_type(account_type());
}

wxString
Account::name()
{
	load();
	return value(m_data->name);
}

Handle<Commodity>
Account::commodity()
{
	load();
	return value(m_data->commodity);
}

wxString
Account::description()
{
	load();
	return value(m_data->description);
}

Visibility
Account::visibility()
{
	load();
	return value(m_data->visibility);
}

Decimal
Account::technical_balance()
{
	load();  // This may be unnecessary but there's no harm in it.
	return BalanceCacheAttorney::technical_balance
	(	database_connection(),
		id()
	);
}

Decimal
Account::friendly_balance()
{
	load();
	return technical_to_friendly(technical_balance(), account_super_type());
}

Decimal
Account::technical_opening_balance()
{
	load();
	return BalanceCacheAttorney::technical_opening_balance
	(	database_connection(),
		id()
	);
}

Decimal
Account::friendly_opening_balance()
{
	load();
	return technical_to_friendly
	(	technical_opening_balance(),
		account_super_type()
	);
}

Decimal
Account::budget()
{
	load();
	return BudgetAttorney::budget
	(	database_connection(),
		id()
	);
}

vector<Handle<BudgetItem> >
Account::budget_items()
{
	load();
	vector<Handle<BudgetItem> > ret;
	SQLStatement s
	(	database_connection(),
		"select budget_item_id from budget_items where "
		"account_id = :p"
	);
	s.bind(":p", id());
	JEWEL_ASSERT (ret.empty());
	while (s.step())
	{
		Handle<BudgetItem> const bi(database_connection(), s.extract<Id>(0));
		ret.push_back(bi);
	}
	return ret;
}

void
Account::set_account_type(AccountType p_account_type)
{
	load();
	m_data->account_type = p_account_type;
	return;
}

void
Account::set_name(wxString const& p_name)
{
	load();
	m_data->name = p_name;
	return;
}

void
Account::set_commodity(Handle<Commodity> const& p_commodity)
{
	load();
	m_data->commodity = p_commodity;
	return;
}

void
Account::set_description(wxString const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void
Account::set_visibility(Visibility p_visibility)
{
	load();
	m_data->visibility = p_visibility;
	return;
}

void
Account::swap(Account& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
Account::do_load()
{
	SQLStatement statement
	(	database_connection(),
		"select name, commodity_id, account_type_id, "
		"description, visibility_id "
		"from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	Account temp(*this);
	temp.m_data->name = std8_to_wx(statement.extract<string>(0));
	temp.m_data->commodity = Handle<Commodity>
	(	database_connection(),
		statement.extract<Id>(1)
	);
	temp.m_data->account_type =
		static_cast<AccountType>(statement.extract<int>(2));
	temp.m_data->description = std8_to_wx(statement.extract<string>(3));
	temp.m_data->visibility =
		static_cast<Visibility>(statement.extract<int>(4));
	swap(temp);
	return;
}

void
Account::process_saving_statement(SQLStatement& statement)
{
	statement.bind
	(	":account_type_id",
		static_cast<int>(value(m_data->account_type))
	);
	statement.bind(":name", wx_to_std8(value(m_data->name)));
	statement.bind
	(	":description",
		wx_to_std8(value(m_data->description))
	);
	statement.bind(":commodity_id", value(m_data->commodity)->id());
	statement.bind
	(	":visibility_id",
		static_cast<int>(value(m_data->visibility))
	);
	statement.step_final();
	return;
}

void
Account::do_save_existing()
{
	BalanceCacheAttorney::mark_as_stale(database_connection(), id());
	SQLStatement updater
	(	database_connection(),
		"update accounts set "
		"name = :name, "
		"commodity_id = :commodity_id, "
		"account_type_id = :account_type_id, "
		"description = :description, "
		"visibility_id = :visibility_id "
		"where account_id = :account_id"
	);
	updater.bind(":account_id", id());
	process_saving_statement(updater);
	BudgetAttorney::regenerate(database_connection());
	return;
}

void
Account::do_save_new()
{
	BalanceCacheAttorney::mark_as_stale(database_connection());
	SQLStatement inserter
	(	database_connection(),
		"insert into accounts"
		"("
			"account_type_id, "
			"name, "
			"description, "
			"commodity_id, "
			"visibility_id"
		") "
		"values"
		"("
			":account_type_id, "
			":name, "
			":description, "
			":commodity_id, "
			":visibility_id"
		")"
	);
	process_saving_statement(inserter);
	BudgetAttorney::regenerate(database_connection());
	return;
}

void
Account::do_remove()
{
	if (id() == database_connection().balancing_account()->id())
	{
		JEWEL_THROW
		(	PreservedRecordDeletionException,
			"Budget balancing account cannot be deleted."
		);
	}
	BalanceCacheAttorney::mark_as_stale(database_connection(), id());
	string const statement_text =
		"delete from " + primary_table_name() + " where " +
		primary_key_name() + " = :p";
	SQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());
	statement.step_final();
	BudgetAttorney::regenerate(database_connection());
	return;
}

void
Account::do_ghostify()
{
	JEWEL_LOG_TRACE();
	clear(m_data->name);
	clear(m_data->commodity);
	clear(m_data->account_type);
	clear(m_data->description);
	clear(m_data->visibility);
	return;
}

string
Account::exclusive_table_name()
{
	return "accounts";
}

string
Account::primary_key_name()
{
	return "account_id";
}

map<AccountSuperType, sqloxx::Id>
favourite_accounts(PhatbooksDatabaseConnection& p_database_connection)
{
	map<AccountSuperType, sqloxx::Id> ret;
	map<sqloxx::Id, size_t> account_map;
	AccountTableIterator a_it(p_database_connection);
	AccountTableIterator a_end;
	for ( ; a_it != a_end; ++a_it)
	{
		JEWEL_ASSERT ((*a_it)->has_id());
		account_map[(*a_it)->id()] = 0;
	}
	SQLStatement selector
	(	p_database_connection,
		"select account_id, count(journal_id) from "
		"("
			"select account_id, journal_id from entries join "
			"ordinary_journal_detail using(journal_id) join journals "
			"using(journal_id) where transaction_type_id != :natt and "
			"date >= :min_date order by date"
		") "
		"group by account_id;"
	);
	selector.bind(":natt", static_cast<int>(non_actual_transaction_type()));
	selector.bind(":min_date", julian_int(today()) - 30);
	while (selector.step())
	{
		account_map[selector.extract<sqloxx::Id>(0)] =
			selector.extract<long long>(1);
	}
	map<AccountSuperType, size_t> max_counts;
	for (AccountSuperType ast: account_super_types())
	{
		max_counts[ast] = 0;
	}
	Handle<Account> const balancing_acct =
		p_database_connection.balancing_account();
	for (auto const& account_map_elem: account_map)
	{
		Handle<Account> const account
		(	p_database_connection,
			account_map_elem.first
		);
		size_t const count = account_map_elem.second;
		AccountSuperType const stype =
			super_type(account->account_type());
		if
		(	(	(account_map[account->id()] >= max_counts[stype]) ||
				(ret[stype] == balancing_acct->id())
			)
			&&
			(	(account != balancing_acct)
			)
		)
		{
			JEWEL_ASSERT (account->has_id());
			JEWEL_ASSERT (account_map_elem.first == account->id());
			ret[stype] = account->id();
			max_counts[stype] = count;
		}
	}
	return ret;
}

wxString account_concept_name
(	AccountSuperType p_account_super_type,
	AccountPhraseFlags p_phrase_flag_set
)
{
	wxString ret;
	JEWEL_ASSERT (ret.IsEmpty());
	if (p_phrase_flag_set.test(string_flags::include_article))
	{
		ret += wxString("an ");
	}
	bool const capitalize = p_phrase_flag_set.test(string_flags::capitalize);
	switch (p_account_super_type)
	{
	case AccountSuperType::balance_sheet:
		ret +=
		(	capitalize?
			wxString("Account"):
			wxString("account")
		);
		break;
	case AccountSuperType::pl:
		ret +=
		(	capitalize?
			wxString("Envelope"):
			wxString("envelope")
		);
		break;
	default:
		JEWEL_HARD_ASSERT (false);
	}
	if (p_phrase_flag_set.test(string_flags::pluralize))
	{
		ret += wxString("s");
	}
	return ret;
}

wxString account_concepts_phrase
(	AccountPhraseFlags p_phrase_flag_set
)
{
	wxString ret = account_concept_name
	(	AccountSuperType::balance_sheet,
		p_phrase_flag_set
	);
	ret += wxString(" or ");
	p_phrase_flag_set.clear(string_flags::include_article);
	ret += account_concept_name
	(	AccountSuperType::pl,
		p_phrase_flag_set
	);
	return ret;
}


}  // namespace phatbooks
