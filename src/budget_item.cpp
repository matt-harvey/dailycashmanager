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


#include "budget_item.hpp"
#include "account.hpp"
#include "commodity.hpp"
#include "frequency.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_conv.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/decimal.hpp>
#include <jewel/log.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <sqloxx/id.hpp>
#include <sqloxx/identity_map.hpp>
#include <sqloxx/sql_statement.hpp>
#include <wx/string.h>
#include <string>
#include <vector>

using boost::optional;
using jewel::Decimal;
using jewel::clear;
using jewel::value;
using jewel::UninitializedOptionalException;
using sqloxx::Handle;
using sqloxx::Id;
using sqloxx::IdentityMap;
using sqloxx::SQLStatement;
using std::string;
using std::vector;

namespace phatbooks
{

typedef
	PhatbooksDatabaseConnection::BudgetAttorney
	BudgetAttorney;


struct BudgetItem::BudgetItemData
{
	optional<Handle<Account> > account;
	optional<wxString> description;
	optional<Frequency> frequency;
	optional<Decimal> amount;
};

void
BudgetItem::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	dbc.execute_sql
	(	"create table budget_items"
		"("
			"budget_item_id integer primary key autoincrement, "
			"account_id not null references accounts, "
			"description text, "
			"interval_units integer not null, "
			"interval_type_id integer not null references interval_types, "
			"amount integer not null"
		");"
	);
	return;
}
	
BudgetItem::BudgetItem
(	IdentityMap& p_identity_map,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map),
	m_data(new BudgetItemData)
{
	(void)p_signature;  // silence compiler re. unused parameter.
}

BudgetItem::BudgetItem
(	IdentityMap& p_identity_map,
	sqloxx::Id p_id,
	IdentityMap::Signature const& p_signature
):
	PersistentObject(p_identity_map, p_id),
	m_data(new BudgetItemData)
{
	(void)p_signature;  // silence compiler re. unused parameter.
}

BudgetItem::~BudgetItem() = default;

std::string
BudgetItem::exclusive_table_name()
{
	return "budget_items";
}

std::string
BudgetItem::primary_key_name()
{
	return "budget_item_id";	
}

void
BudgetItem::mimic(BudgetItem& rhs)
{
	load();
	BudgetItem temp(*this);
	temp.set_description(rhs.description());
	temp.set_account(rhs.account());
	temp.set_frequency(rhs.frequency());
	temp.set_amount(rhs.amount());
	swap(temp);
	return;
}

BudgetItem::BudgetItem(BudgetItem const& rhs):
	PersistentObject(rhs),
	m_data(new BudgetItemData(*(rhs.m_data)))
{
}

void
BudgetItem::set_description(wxString const& p_description)
{
	load();
	m_data->description = p_description;
	return;
}

void
BudgetItem::set_account(Handle<Account> const& p_account)
{
	load();
	m_data->account = p_account;
	return;
}

void
BudgetItem::set_frequency(Frequency const& p_frequency)
{
	load();
	m_data->frequency = p_frequency;
	return;
}

void
BudgetItem::set_amount(Decimal const& p_amount)
{
	load();
	m_data->amount = p_amount;
	return;
}

wxString
BudgetItem::description()
{
	load();
	return value(m_data->description);
}

Handle<Account>
BudgetItem::account()
{
	load();
	return value(m_data->account);
}

Frequency
BudgetItem::frequency()
{
	load();
	return value(m_data->frequency);
}

Decimal
BudgetItem::amount()
{
	load();
	return value(m_data->amount);
}

void
BudgetItem::swap(BudgetItem& rhs)
{
	swap_base_internals(rhs);
	using std::swap;
	swap(m_data, rhs.m_data);
	return;
}

void
BudgetItem::do_load()
{
	BudgetItem temp(*this);
	SQLStatement statement
	(	database_connection(),
		"select account_id, description, interval_units, interval_type_id, "
		"amount from budget_items where budget_item_id = :p"
	);
	statement.bind(":p", id());
	statement.step();
	sqloxx::Id const acct_id =  statement.extract<sqloxx::Id>(0);
	Handle<Account> const acct(database_connection(), acct_id);
	Decimal const amt
	(	statement.extract<Decimal::int_type>(4),
		acct->commodity()->precision()
	);
	temp.m_data->account = Handle<Account>(database_connection(), acct_id);
	temp.m_data->description = std8_to_wx(statement.extract<string>(1));
	temp.m_data->frequency =
	Frequency
	(	statement.extract<int>(2),
		static_cast<IntervalType>(statement.extract<int>(3))
	);
	temp.m_data->amount = amt;
	swap(temp);
}

void
BudgetItem::process_saving_statement(SQLStatement& statement)
{
	JEWEL_ASSERT (value(m_data->account)->has_id());
	ensure_pl_only_budget();
	statement.bind(":account_id", value(m_data->account)->id());
	statement.bind
	(	":description",
		wx_to_std8(value(m_data->description))
	);
	Frequency const freq = value(m_data->frequency);
	statement.bind(":interval_units", freq.num_steps());
	statement.bind(":interval_type_id", static_cast<int>(freq.step_type()));
	statement.bind(":amount", value(m_data->amount).intval());
	statement.step_final();
	return;
}

void
BudgetItem::ensure_pl_only_budget()
{
	Handle<Account> acct;
	try
	{
		acct = account();
	}
	catch (UninitializedOptionalException&)
	{
		// do nothing - OK as Account not yet initalized
		return;
	}
	if (acct->account_super_type() != AccountSuperType::pl)
	{
		JEWEL_THROW
		(	InvalidBudgetItemException,
			"Account of BudgetItem should be of AccountSuperType::pl."
		);
	}
	return;
}

void
BudgetItem::do_save_existing()
{
	SQLStatement updater
	(	database_connection(),
		"update budget_items set "
		"account_id = :account_id, "
		"description = :description, "
		"interval_units = :interval_units, "
		"interval_type_id = :interval_type_id, "
		"amount = :amount "
		"where budget_item_id = :budget_item_id"
	);
	updater.bind(":budget_item_id", id());
	process_saving_statement(updater);
	BudgetAttorney::regenerate(database_connection());
	return;
}

void
BudgetItem::do_save_new()
{
	SQLStatement inserter
	(	database_connection(),
		"insert into budget_items"
		"("
			"account_id, "
			"description, "
			"interval_units, "
			"interval_type_id, "
			"amount"
		") "
		"values"
		"("
			":account_id, "
			":description, "
			":interval_units, "
			":interval_type_id, "
			":amount"
		")"
	);
	process_saving_statement(inserter);
	BudgetAttorney::regenerate(database_connection());
	return;
}
			
void
BudgetItem::do_ghostify()
{
	clear(m_data->account);
	clear(m_data->description);
	clear(m_data->frequency);
	clear(m_data->amount);
	return;
}

void
BudgetItem::do_remove()
{
	string const statement_text =
		"delete from " + primary_table_name() + " where " +
		primary_key_name() + " = :p";
	SQLStatement statement(database_connection(), statement_text);
	statement.bind(":p", id());
	statement.step_final();
	BudgetAttorney::regenerate(database_connection());
	return;
}

Decimal
normalized_total
(	vector<Handle<BudgetItem> >::const_iterator b,
	vector<Handle<BudgetItem> >::const_iterator const& e
)
{
	JEWEL_ASSERT (e - b > 0);  // Assert precondition.
	PhatbooksDatabaseConnection& dbc = (*b)->database_connection();
	Handle<Commodity> commodity(dbc);

	// TODO LOW PRIORITY The below is a temporary hack - if Accounts can ever
	// have Commodities other than the default Commodity, then this will no
	// longer work. Either make this less hacky, or else at least do an
	// assertion or throw if there are non-default Commodities.
	try
	{
		commodity = (*b)->account()->commodity();
	}
	catch (UninitializedOptionalException&)
	{
		commodity = dbc.default_commodity();
	}
	Decimal::places_type const prec = commodity->precision();
	Decimal ret(0, prec);
	for ( ; b != e; ++b)
	{
		JEWEL_ASSERT
		(	(*b)->database_connection().
				supports_budget_frequency((*b)->frequency())
		);
		ret += convert_to_canonical((*b)->frequency(), (*b)->amount());
	}
	return round(convert_from_canonical(dbc.budget_frequency(), ret), prec);
}

}  // namespace phatbooks
