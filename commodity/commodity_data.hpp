#ifndef GUARD_commodity_data_hpp_4995138084048004
#define GUARD_commodity_data_hpp_4995138084048004

#include "commodity.hpp"
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <wx/string.h>

namespace phatbooks
{

struct Commodity::CommodityData
{
	boost::optional<wxString> abbreviation;
	boost::optional<wxString> name;
	boost::optional<wxString> description;
	boost::optional<int> precision;
	boost::optional<jewel::Decimal> multiplier_to_base;
};

}  // namespace phatbooks

#endif  // GUARD_commodity_data_hpp_4995138084048004
