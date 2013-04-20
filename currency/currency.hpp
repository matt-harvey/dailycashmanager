#ifndef GUARD_currency_hpp
#define GUARD_currency_hpp

#include "commodity.hpp"
#include <boost/noncopyable.hpp>
#include <vector>


namespace phatbooks
{

class PhatbooksDatabaseConnection;


/**
 * Presents a list of almost all currencies in the world, as
 * Commodity objects in a std::vector.
 */
class CurrencyManager: boost::noncopyable
{
public:
	CurrencyManager(PhatbooksDatabaseConnection& p_database_connection);
	~CurrencyManager();
	std::vector<Commodity> const& currencies() const;
private:
	PhatbooksDatabaseConnection& m_database_connection;
	std::vector<Commodity>* m_currencies;
};

}  // namespace phatbooks

#endif  // GUARD_currency_hpp
