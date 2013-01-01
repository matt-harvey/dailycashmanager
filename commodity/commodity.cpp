#include "commodity.hpp"
#include "commodity_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::Handle;
using std::string;

namespace phatbooks
{


void
Commodity::setup_tables(PhatbooksDatabaseConnection& dbc)
{
	CommodityImpl::setup_tables(dbc);
	return;
}

Commodity::Commodity
(	PhatbooksDatabaseConnection& p_database_connection
):
	PhatbooksPersistentObjectDetail(p_database_connection)
{
}

Commodity::Commodity
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObjectDetail(p_database_connection, p_id)
{
}


Commodity
Commodity::create_unchecked
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
)
{
	return Commodity
	(	Handle<CommodityImpl>::create_unchecked
		(	p_database_connection,
			p_id
		)
	);
}


Commodity::Commodity
(	PhatbooksDatabaseConnection& p_database_connection,
	string const& p_abbreviation
):
	PhatbooksPersistentObjectDetail
	(	p_database_connection,
		CommodityImpl::id_for_abbreviation
		(	p_database_connection,
			p_abbreviation
		)
	)
{
}
string
Commodity::abbreviation() const
{
	return impl().abbreviation();
}

string
Commodity::name() const
{
	return impl().name();
}

std::string
Commodity::description() const
{
	return impl().description();
}

int
Commodity::precision() const
{
	return impl().precision();
}

Decimal
Commodity::multiplier_to_base() const
{
	return impl().multiplier_to_base();
}

void
Commodity::set_abbreviation(string const& p_abbreviation)
{
	impl().set_abbreviation(p_abbreviation);
	return;
}

void
Commodity::set_name(std::string const& p_name)
{
	impl().set_name(p_name);
	return;
}

void
Commodity::set_description(std::string const& p_description)
{
	impl().set_description(p_description);
	return;
}

void
Commodity::set_precision(int p_precision)
{
	impl().set_precision(p_precision);
	return;
}

void
Commodity::set_multiplier_to_base(Decimal const& p_multiplier_to_base)
{
	impl().set_multiplier_to_base(p_multiplier_to_base);
	return;
}

Commodity::Commodity(sqloxx::Handle<CommodityImpl> const& p_handle):
	PhatbooksPersistentObjectDetail(p_handle)
{
}



}  // namespace phatbooks
