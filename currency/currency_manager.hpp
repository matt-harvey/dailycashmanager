#ifndef GUARD_currency_manager_hpp
#define GUARD_currency_manager_hpp

#include "b_string.hpp"
#include "commodity.hpp"
#include <boost/noncopyable.hpp>
#include <vector>


namespace phatbooks
{

class PhatbooksDatabaseConnection;


/**
 * Presents a list of almost all currencies in the world, as
 * Commodity objects in a std::vector.
 *
 * @todo The Japanese yen is officially divided into 100 sen; but a single
 * sen is of minuscule value, and it in everyday life, divisions smaller
 * than 1 yen are not used. Perhaps this is the case with other currencies.
 * Do we want to allow for this, and store some currencies in amounts
 * less precise than their official precision?
 */
class CurrencyManager: boost::noncopyable
{
public:
	CurrencyManager(PhatbooksDatabaseConnection& p_database_connection);
	~CurrencyManager();

	/**
	 * @returns a reference to a container of Commodities, representing
	 * almost all the currencies of the world. For these Commodities
	 * multiplier_to_base() will be uninitialized, but all other attributes
	 * will be initialized.
	 */
	std::vector<Commodity> const& currencies() const;

	/**
	 * @returns the Commodity with p_abbreviation as its abbreviation(),
	 * from among those stored in CurrencyManager. There is guaranteed
	 * to be no more than one Commodity stored with a given abbreviation.
	 * 
	 * @throws CurrencyManagerException if there is no Commodity with this
	 * abbreviation in the CurrencyManager.
	 *
	 * @todo Enforce the abovementioned guarantee.
	 */
	Commodity
	get_currency_with_abbreviation(BString const& p_abbreviation) const;


private:
	PhatbooksDatabaseConnection& m_database_connection;
	std::vector<Commodity>* m_currencies;
};

}  // namespace phatbooks

#endif  // GUARD_currency_manager_hpp
