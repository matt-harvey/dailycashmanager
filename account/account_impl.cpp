// Copyright (c) 2013, Matthew Harvey. All rights reserved.

/** \file account_impl.cpp
 *
 * \brief Source file for code pertaining to AccountImpl class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */

#include "account_impl.hpp"
#include "account.hpp"
#include "account_table_iterator.hpp"
#include "account_type.hpp"
#include "date.hpp"
#include "string_conv.hpp"
#include "budget_item.hpp"
#include "commodity.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "transaction_type.hpp"
#include "visibility.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sql_statement.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <wx/string.h>
#include <algorithm>
#include <map>
#include <string>
#include <vector>

using boost::numeric_cast;
using boost::shared_ptr;
using jewel::clear;
using jewel::Decimal;
using jewel::value;
using sqloxx::SQLStatement;
using std::find_if;
using std::map;
using std::string;
using std::vector;

namespace gregorian = boost::gregorian;


namespace phatbooks
{


typedef
	PhatbooksDatabaseConnection::BalanceCacheAttorney
	BalanceCacheAttorney;

typedef
	PhatbooksDatabaseConnection::BudgetAttorney
	BudgetAttorney;



void
AccountImpl::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table visibilities"
		"("
			"visibility_id integer primary key"
		");"
	);
	using visibility::Visibility;
	for
	(	int i = 0;
		i != static_cast<int>(visibility::num_visibilities);
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

AccountImpl::Id
AccountImpl::id_for_name
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
			return statement.extract<AccountImpl::Id>(0);
		}
	}
	JEWEL_THROW
	(	InvalidAccountNameException,
		"There is no AccountImpl with the passed name."
	);
}

AccountImpl::AccountImpl
(	IdentityMap& p_identity_map	
):
	PersistentObject(p_identity_map),
	m_data(new AccountData)
{
}

AccountImpl::AccountImpl
(	IdentityMap& p_identity_map,	
	Id p_id
):
	PersistentObject(p_identity_map, p_id),
	m_data(new AccountData)
{
}

AccountImpl::AccountImpl(AccountImpl const& rhs):
	PersistentObject(rhs),
	m_data(new AccountData(*(rhs.m_data)))
{
}

AccountImpl::~AccountImpl()
{
}

bool
AccountImpl::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return PersistentObject::exists(p_database_connection, p_id);
}

bool
AccountImpl::exists
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
)
{
	wxString const target = p_name.Lower();
	SQLStatement statement
	(	p_database_connection,
		"select name from accounts"
	);
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
AccountImpl::no_user_pl_accounts_saved
(	PhatbooksDatabaseConnection& p_database_connection
)
{
	AccountTableIterator it(p_database_connection);
	AccountTableIterator const end;
	if (it == end)
	{
		return true;
	}
	Account const bal_account = p_database_connection.balancing_account();
	for ( ; it != end; ++it)
	{
		if
		(	(super_type(it->account_type()) == account_super_type::pl) &&
			(*it != bal_account)
		)
		{
			return false;
		}
	}
	return true;
}

bool
AccountImpl::none_saved_with_account_type
(	PhatbooksDatabaseConnection& p_database_connection,
	account_type::AccountType p_account_type
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
AccountImpl::none_saved_with_account_super_type
(	PhatbooksDatabaseConnection& p_database_connection,
	account_super_type::AccountSuperType p_account_super_type
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


AccountImpl::AccountType
AccountImpl::account_type()
{
	load();
	return value(m_data->account_type);
}

AccountImpl::AccountSuperType
AccountImpl::account_super_type()
{
	load();
	return super_type(account_type());
}

wxString
AccountImpl::name()
{
	load();
	return value(m_data->name);
}

Commodity
AccountImpl::commodity()
{
	load();
	return value(m_data->commodity);
}

wxString
AccountImpl::description()
{
	load();
	return value(m_data->description);
}

visibility::Visibility
AccountImpl::visibility()
{
	load();
	return value(m_data->visibility);
}

namespace
{
	// Convert a "technical balance" to a "friendly balance",
	// where the balance is the balance of an Account with
	// account_super_type() ast.
	Decimal technical_to_friendly
	(	Decimal const& d,
		account_super_type::AccountSuperType ast
	)
	{
		switch (ast)
		{
		case account_super_type::balance_sheet:
			return d;
		case account_super_type::pl:
			return round(d * Decimal(-1, 0), d.places());
		default:
			JEWEL_HARD_ASSERT (false);
		}
	}
}  // end anonymous namespace


Decimal
AccountImpl::technical_balance()
{
	load();  // This may be unnecessary but there's no harm in it.
	return BalanceCacheAttorney::technical_balance
	(	database_connection(),
		id()
	);
}

Decimal
AccountImpl::friendly_balance()
{
	load();
	return technical_to_friendly(technical_balance(), account_super_type());
}

Decimal
AccountImpl::technical_opening_balance()
{
	load();
	return BalanceCacheAttorney::technical_opening_balance
	(	database_connection(),
		id()
	);
}

Decimal
AccountImpl::friendly_opening_balance()
{
	load();
	return technical_to_friendly
	(	technical_opening_balance(),
		account_super_type()
	);
}

Decimal
AccountImpl::budget()
{
	load();
	return BudgetAttorney::budget
	(	database_connection(),
		id()
	);
}

vector<BudgetItem>
AccountImpl::budget_items()
{
	load();
	vector<BudgetItem> ret;
	SQLStatement s
	(	database_connection(),
		"select budget_item_id from budget_items where "
		"account_id = :p"
	);
	s.bind(":p", id());
	JEWEL_ASSERT (ret.empty());
	while (s.step())
	{
		BudgetItem bi(database_connection(), s.extract<BudgetItem::Id>(0));
		ret.push_back(bi);
	}
	return ret;
}

void
AccountImpl::set_account_type(AccountType p_account_type)
{
	load();
	m_data->account_type = p_account_type;
	return;
}

void
AccountImpl::set_name(wxString const& p_name)
{
	load();
	m_data->name = p_name;
	return;
}

void
AccountImpl::set_commodity(Commodity const& p_commodity)
{
	load();
	m_data->commodity = p_commodity;
	return;
}

void
AccountImpl::set_description(wxString const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void
AccountImpl::set_visibility(visibility::Visibility p_visibility)
{
	load();
	m_data->visibility = p_visibility;
	return;
}

void
AccountImpl::swap(AccountImpl& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
AccountImpl::do_load()
{
	SQLStatement statement
	(	database_connection(),
		"select name, commodity_id, account_type_id, "
		"description, visibility_id "
		"from accounts where account_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	AccountImpl temp(*this);
	temp.m_data->name = std8_to_wx(statement.extract<string>(0));
	temp.m_data->commodity = Commodity
	(	database_connection(),
		statement.extract<Id>(1)
	);
	temp.m_data->account_type =
		static_cast<AccountType>(statement.extract<int>(2));
	temp.m_data->description = std8_to_wx(statement.extract<string>(3));
	temp.m_data->visibility =
		static_cast<visibility::Visibility>(statement.extract<int>(4));
	swap(temp);
	return;
}

void
AccountImpl::process_saving_statement(SQLStatement& statement)
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
	statement.bind(":commodity_id", value(m_data->commodity).id());
	statement.bind
	(	":visibility_id",
		static_cast<int>(value(m_data->visibility))
	);
	statement.step_final();
	return;
}

void
AccountImpl::do_save_existing()
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
AccountImpl::do_save_new()
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
AccountImpl::do_remove()
{
	if (id() == database_connection().balancing_account().id())
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
AccountImpl::do_ghostify()
{
	// WARNING Is there any situation in which this should
	// mark the balance cache as stale?
	// I don't think so, but if there is,
	// note that marking the balance cache
	// as stale here might perhaps throw an exception (which is contrary
	// to the Sqloxx API which requires do_ghostify() to be
	// non-throwing).
	clear(m_data->name);
	clear(m_data->commodity);
	clear(m_data->account_type);
	clear(m_data->description);
	clear(m_data->visibility);
	return;
}


string
AccountImpl::primary_table_name()
{
	return "accounts";
}

string
AccountImpl::exclusive_table_name()
{
	return primary_table_name();
}

string
AccountImpl::primary_key_name()
{
	return "account_id";
}

void
favourite_accounts
(	PhatbooksDatabaseConnection& p_database_connection,
	std::map<account_super_type::AccountSuperType, AccountImpl::Id>& out
)
{
	// TODO Code is duplicated between here and "entry_table_iterator.cpp".
#	ifndef NDEBUG
		// Ensure we are picking all and only the
		// actual transactions.
		int const target_non_actual_type = 3;
		int i = 0;
		int const lim =
			static_cast<int>(transaction_type::num_transaction_types);
		for ( ; i != lim; ++i)
		{
			transaction_type::TransactionType const ttype =
				static_cast<transaction_type::TransactionType>(i);
			if (ttype == target_non_actual_type)
			{
				JEWEL_ASSERT (!transaction_type_is_actual(ttype));
			}
			else
			{
				JEWEL_ASSERT (transaction_type_is_actual(ttype));
			}
		}
#	endif

	map<Account::Id, size_t> account_map;
	AccountTableIterator a_it(p_database_connection);
	AccountTableIterator a_end;
	for ( ; a_it != a_end; ++a_it)
	{
		JEWEL_ASSERT (a_it->has_id());
		account_map[a_it->id()] = 0;
	}
	SQLStatement selector
	(	p_database_connection,
		"select account_id, count(journal_id) from "
		"("
			"select account_id, journal_id from entries join "
			"ordinary_journal_detail using(journal_id) join journals "
			"using(journal_id) where transaction_type_id != 3 and "
			"date >= :min_date order by date"
		") "
		"group by account_id;"
	);
	selector.bind(":min_date", julian_int(today()) - 30);
	while (selector.step())
	{
		account_map[selector.extract<Account::Id>(0)] =
			selector.extract<long long>(1);
	}
	map<account_super_type::AccountSuperType, size_t> max_counts;
	vector<account_super_type::AccountSuperType> const& super_types =
		account_super_types();
	for
	(	vector<account_super_type::AccountSuperType>::size_type j = 0;
		j != super_types.size();
		++j
	)
	{
		max_counts[super_types[j]] = 0;
	}
	map<Account::Id, size_t>::const_iterator it = account_map.begin();
	map<Account::Id, size_t>::const_iterator const end = account_map.end();
	Account const balancing_acct = p_database_connection.balancing_account();
	for ( ; it != end; ++it)
	{
		Account const account(p_database_connection, it->first);
		size_t const count = it->second;
		account_super_type::AccountSuperType const stype =
			super_type(account.account_type());
		if
		(	(	(account_map[account.id()] >= max_counts[stype]) ||
				(out[stype] == balancing_acct.id())
			)
			&&
			(	(account != balancing_acct)
			)
		)
		{
			JEWEL_ASSERT (account.has_id());
			JEWEL_ASSERT (it->first == account.id());
			out[stype] = account.id();
			max_counts[stype] = count;
		}
	}
	return;
}




}  // namespace phatbooks
