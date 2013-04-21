#include "currency_manager.hpp"
#include "b_string.hpp"
#include "commodity.hpp"
#include "phatbooks_exceptions.hpp"
#include <wx/string.h>
#include <cassert>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace phatbooks
{

namespace
{
	Commodity make_currency
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_name,  // So we can accept wide string literals
		string const& p_abbreviation,
		int p_precision
	)
	{
		Commodity ret(p_database_connection);
		ret.set_name(wx_to_bstring(p_name));
		ret.set_abbreviation(std8_to_bstring(p_abbreviation));
		ret.set_description(BString(""));
		ret.set_precision(p_precision);
		return ret;
	}

	void make_currencies
	(	PhatbooksDatabaseConnection& dbc,
		vector<Commodity>& container
	)
	{
		// currency.inl contains repetitive code generated from a csv
		// file via a script.
		#include "currency_manager.inc"
		return;
	}

}  // end anonymous namespace



CurrencyManager::CurrencyManager
(	PhatbooksDatabaseConnection& p_database_connection
):
	m_database_connection(p_database_connection),
	m_currencies(0)
{
	m_currencies = new vector<Commodity>;
	m_currencies->reserve(200);
	make_currencies(m_database_connection, *m_currencies);
}

CurrencyManager::~CurrencyManager()
{
	delete m_currencies;
	m_currencies = 0;
}

vector<Commodity> const&
CurrencyManager::currencies() const
{
	assert (m_currencies);
	return *m_currencies;
}

Commodity
CurrencyManager::get_commodity_with_abbreviation
(	BString const& p_abbreviation
) const
{
	for
	(	vector<Commodity>::const_iterator it = m_currencies->begin(),
			end = m_currencies->end();
			it != end;
			++it
	)
	{
		if (it->abbreviation() == p_abbreviation)
		{
			return *it;
		}
	}
	throw CurrencyManagerException
	(	"No Commodity in CurrencyManager has this abbreviation."
	);
}
	
		




}  // namespace phatbooks
