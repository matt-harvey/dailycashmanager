#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>


namespace phatbooks
{

class CommodityImpl;
class PhatbooksDatabaseConnection;



class Commodity
{
public:
	typedef sqloxx::Id Id;
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit Commodity
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * Get a Commodity by id from the database.
	 */
	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		std::string const& p_abbreviation
	);

	/**
	 * Get a Commodity by id from the database, without
	 * the safety feature of throwing if an
	 * Account with this id does not exist.
	 */
	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		std::string const& p_abbreviation,
		int p_dummy
	);

	std::string abbreviation() const;
	std::string name() const;
	std::string description() const;
	int precision() const;
	jewel::Decimal multiplier_to_base() const;
	void set_abbreviation(std::string const& p_abbreviation);
	void set_name(std::string const& p_name);
	void set_description(std::string const& p_description);
	void set_precision(int p_precision);
	void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

	// TODO These should be moved into a base class in due course
	void save();
	Id id() const;

private:
	sqloxx::Handle<CommodityImpl> m_impl;

};


}  // namespace phatbooks


#endif  // GUARD_commodity_hpp
