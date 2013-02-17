#include "account.hpp"
#include "account_type.hpp"
#include "account_impl.hpp"
#include "account_reader.hpp"
#include "commodity.hpp"
#include "finformat.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include "string_conv.hpp"
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <wx/string.h>
#include <string>
#include <vector>

using phatbooks::account_type::AccountType;
using sqloxx::Handle;
using boost::shared_ptr;
using jewel::Decimal;
using std::string;
using std::vector;


namespace phatbooks
{

using string_conv::wx_to_std8;


void
Account::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	AccountImpl::setup_tables(dbc);
	return;
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection
):
	PhatbooksPersistentObject(p_database_connection)
{
}


Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
{
}

Account::Account
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
):
	PhatbooksPersistentObject
	(	p_database_connection,
		AccountImpl::id_for_name(p_database_connection, p_name)
	)
{
}

Account::Account(sqloxx::Handle<AccountImpl> const& p_handle):
	PhatbooksPersistentObject(p_handle)
{
}

Account
Account::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Account
	(	sqloxx::Handle<AccountImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}

Account::~Account()
{
}


wxString
Account::name() const
{
	return impl().name();
}

Commodity
Account::commodity() const
{
	return impl().commodity();
}

AccountType
Account::account_type() const
{
	return impl().account_type();
}

wxString
Account::description() const
{
	return impl().description();
}

Decimal
Account::technical_balance() const
{
	return impl().technical_balance();
}

Decimal
Account::friendly_balance() const
{
	return impl().friendly_balance();
}

void
Account::set_account_type(AccountType p_account_type)
{
	impl().set_account_type(p_account_type);
	return;
}

void
Account::set_name(wxString const& p_name)
{
	impl().set_name(p_name);
	return;
}

void
Account::set_commodity(Commodity const& p_commodity)
{
	impl().set_commodity(p_commodity);
	return;
}

void
Account::set_description(wxString const& p_description)
{
	impl().set_description(p_description);
	return;
}

shared_ptr<vector<string> >
make_account_row(Account const& account)
{
	shared_ptr<vector<string> > ret(new vector<string>);
	ret->push_back(wx_to_std8(account.name()));
	ret->push_back(finformat_std8(account.friendly_balance()));
	return ret;
}

shared_ptr<vector<string> >
make_detailed_account_row(Account const& account)
{
	shared_ptr<vector<string> > ret(new vector<string>);
	ret->push_back(wx_to_std8(account.name()));
	ret->push_back(wx_to_std8(account_type_to_string(account.account_type())));
	ret->push_back(wx_to_std8(account.description()));
	;return ret;
}




// Free-standing functions

bool is_asset_or_liability(Account const& account)
{
	bool ret;
	switch (account.account_type())
	{
	case account_type::asset:
	case account_type::liability:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}
	return ret;
}

bool is_expense(Account const& account)
{
	return account.account_type() == account_type::expense;
}

bool is_revenue(Account const& account)
{
	return account.account_type() == account_type::revenue;
}

bool is_envelope(Account const& account)
{
	bool ret;
	switch (account.account_type())
	{
	case account_type::asset:
	case account_type::liability:
		ret = false;
		break;
	case account_type::equity:
	case account_type::revenue:
	case account_type::expense:
	case account_type::pure_envelope:
		ret = true;
		break;
	default:
		assert (false);	
	}
	return ret;
}


bool is_not_pure_envelope(Account const& account)
{
	return account.account_type() != account_type::pure_envelope;
}



}   // namespace phatbooks
