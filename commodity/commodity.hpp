#ifndef GUARD_commodity_hpp
#define GUARD_commodity_hpp

#include "b_string.hpp"
#include "commodity_impl.hpp"
#include "phatbooks_persistent_object.hpp"
#include <sqloxx/general_typedefs.hpp>
#include <sqloxx/handle.hpp>
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>

namespace phatbooks
{

class PhatbooksDatabaseConnection;


/*
 * Class representing commodities, where a commodity is anything of
 * value that can be counted in undifferentiated units, e.g. a particular
 * currency, units in a particular fund, shares in a particular trust,
 * and so forth. A single unique asset, such as a car, might also be
 * regarded as a commodity, of which there is only one unit in
 * existence.
 *
 * @todo The Commodity funcionality of Phatbooks should for the time
 * being remain completely hidden from the user - mainly because it
 * isn't properly implemented yet, because we can't deal with the
 * situation in which there is more than one commodity in the database.
 * That part of the Commodity functionality that has already been
 * written, is for the time being being hidden by the preprocessor.
 * To unhide it, define PHATBOOKS_EXPOSE_COMMODITY.
 */
class Commodity:
	public PhatbooksPersistentObject<CommodityImpl>
{
public:

	typedef
		PhatbooksPersistentObject<CommodityImpl>
		PhatbooksPersistentObject;
	
	typedef PhatbooksPersistentObject::Id Id;

	/**
	 * Sets up tables required in the database for the persistence
	 * of CommodityImpl objects.
	 */
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	/**
	 * Initialize a "draft" Commodity, that will not correspond to
	 * any particular object in the database.
	 */
	explicit Commodity
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	/**
	 * Throws if no such id.
	 */
	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	/**
	 * Faster way of creating a Commodity, but does not check
	 * whether id exists.
	 */
	static Commodity create_unchecked
	(	PhatbooksDatabaseConnection& p_database_connection,
		Id p_id
	);

	Commodity
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_abbreviation
	);

	/**
	 * @returns \c true if and only if \c p_abbreviation is the abbreviation
	 * of a Commodity stored in the database.
	 */
	static bool exists_with_abbreviation
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_abbreviation
	);

	/**
	 * @returns \c true if and only if \c p_name is the name of a Commodity
	 * stored in the database.
	 */
	static bool exists_with_name
	(	PhatbooksDatabaseConnection& p_database_connection,
		BString const& p_name
	);

	/**
	 * Get the abbreviation of the commodity. E.g "AUD" might be
	 * the abbreviation for Australian dollars, or "NAB.AX" might be
	 * the abbreviation for ordinary stock in National Australia Bank
	 * Limited.
	 */
	BString abbreviation() const;

	/**
	 * Get the full name of the commodity. E.g. "Australian dollars".
	 */
	BString name() const;

	/**
	 * Get the description of the commodity, e.g. "notes and coins".
	 */
	BString description() const;

	/**
	 * Get the number of decimal places of precision for the commodity
	 * to which quantities of the commodity are stored.
	 */
	int precision() const;

	/**
	 * Get the multiplier by which 1 unit of the commodity should
	 * be mulitiplied, to be translated into 1 unit of the base commodity
	 * of the entity.
	 */
	jewel::Decimal multiplier_to_base() const;

	void set_abbreviation(BString const& p_abbreviation);
	void set_name(BString const& p_name);
	void set_description(BString const& p_description);
	void set_precision(int p_precision);
	void set_multiplier_to_base(jewel::Decimal const& p_multiplier_to_base);

private:
	Commodity(sqloxx::Handle<CommodityImpl> const& p_handle);

};


}  // namespace phatbooks


#endif  // GUARD_commodity_hpp
