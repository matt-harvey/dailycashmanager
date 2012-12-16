#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include "commodity_impl.hpp"
#include "sqloxx/general_typedefs.hpp"
#include "sqloxx/handle.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>


namespace phatbooks
{

class PhatbooksDatabaseConnection;



class Commodity
{
public:
	typedef sqloxx::Id Id;
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit Commodity
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	// Throws if no such id.
	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	// Fast, unchecked form
	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id,
		char p_dummy
	);

	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		std::string const& p_abbreviation
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
