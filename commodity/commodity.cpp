#include "commodity.hpp"
#include "commodity_impl.hpp"
#include "phatbooks_database_connection.hpp"
#include "sqloxx/database_connection.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

using boost::shared_ptr;
using jewel::Decimal;
using sqloxx::get_handle;
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
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection
):
	m_impl(get_handle<CommodityImpl>(p_database_connection))
{
}

Commodity::Commodity
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection,
	Id p_id
):
	m_impl(get_handle<CommodityImpl>(p_database_connection, p_id))
{
}

Commodity::Commodity
(	shared_ptr<PhatbooksDatabaseConnection> const& p_database_connection,
	string const& p_abbreviation
):
	m_impl
	(	get_handle<CommodityImpl>
		(	p_database_connection,
			CommodityImpl::id_for_abbreviation
			(	*p_database_connection,
				p_abbreviation
			)
		)
	)
{
}


string
Commodity::abbreviation() const
{
	return m_impl->abbreviation();
}

string
Commodity::name() const
{
	return m_impl->name();
}

std::string
Commodity::description() const
{
	return m_impl->description();
}

int
Commodity::precision() const
{
	return m_impl->precision();
}

Decimal
Commodity::multiplier_to_base() const
{
	return m_impl->multiplier_to_base();
}

void
Commodity::set_abbreviation(string const& p_abbreviation)
{
	m_impl->set_abbreviation(p_abbreviation);
	return;
}

void
Commodity::set_name(std::string const& p_name)
{
	m_impl->set_name(p_name);
	return;
}

void
Commodity::set_description(std::string const& p_description)
{
	m_impl->set_description(p_description);
	return;
}

void
Commodity::set_precision(int p_precision)
{
	m_impl->set_precision(p_precision);
	return;
}

void
Commodity::set_multiplier_to_base(Decimal const& p_multiplier_to_base)
{
	m_impl->set_multiplier_to_base(p_multiplier_to_base);
	return;
}

void
Commodity::save()
{
	m_impl->save();
	return;
}

Commodity::Id
Commodity::id() const
{
	return m_impl->id();
}

}  // namespace phatbooks
