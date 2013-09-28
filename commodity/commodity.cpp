// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "commodity.hpp"
#include "commodity_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "phatbooks_persistent_object.hpp"
#include <sqloxx/handle.hpp>
#include <jewel/decimal.hpp>

using jewel::Decimal;
using sqloxx::Handle;
using sqloxx::SQLStatement;

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
	PhatbooksPersistentObject(p_database_connection)
{
}

Commodity::Commodity
(	PhatbooksDatabaseConnection& p_database_connection,
	Id p_id
):
	PhatbooksPersistentObject(p_database_connection, p_id)
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
	wxString const& p_abbreviation
):
	PhatbooksPersistentObject
	(	p_database_connection,
		CommodityImpl::id_for_abbreviation
		(	p_database_connection,
			p_abbreviation
		)
	)
{
}


bool
Commodity::exists_with_abbreviation
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_abbreviation
)
{
	return CommodityImpl::exists_with_abbreviation
	(	p_database_connection,
		p_abbreviation
	);
}


bool
Commodity::exists_with_name
(	PhatbooksDatabaseConnection& p_database_connection,
	wxString const& p_name
)
{
	return CommodityImpl::exists_with_name
	(	p_database_connection,
		p_name
	);
}


wxString
Commodity::abbreviation() const
{
	return impl().abbreviation();
}

wxString
Commodity::name() const
{
	return impl().name();
}

wxString
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
Commodity::set_abbreviation(wxString const& p_abbreviation)
{
	impl().set_abbreviation(p_abbreviation);
	return;
}

void
Commodity::set_name(wxString const& p_name)
{
	impl().set_name(p_name);
	return;
}

void
Commodity::set_description(wxString const& p_description)
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
	PhatbooksPersistentObject(p_handle)
{
}



}  // namespace phatbooks
