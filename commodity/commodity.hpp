#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include "commodity_impl.hpp"
#include "phatbooks_persistent_object.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>


namespace phatbooks
{

class PhatbooksDatabaseConnection;



class Commodity:
	virtual public PhatbooksPersistentObjectBase,
	private PhatbooksPersistentObjectDetail<CommodityImpl>
{
public:

	typedef
		PhatbooksPersistentObjectDetail<CommodityImpl>
		PhatbooksPersistentObjectDetail;
	
	typedef PhatbooksPersistentObjectDetail::Id Id;

	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit Commodity
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	// Throws if no such id.
	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	// Fast, unchecked
	static Commodity create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
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

private:
	Commodity(sqloxx::Handle<CommodityImpl> const& p_handle);

};


}  // namespace phatbooks


#endif  // GUARD_commodity_hpp
