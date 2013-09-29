// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "make_currencies.hpp"
#include "commodity_handle.hpp"
#include "string_conv.hpp"
#include <jewel/assert.hpp>
#include <wx/string.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace phatbooks
{

namespace
{
	CommodityHandle make_currency
	(	PhatbooksDatabaseConnection& p_database_connection,
		wxString const& p_name,  // So we can accept wide string literals
		string const& p_abbreviation,
		int p_precision
	)
	{
		CommodityHandle const ret(p_database_connection);
		ret->set_name(p_name);
		ret->set_abbreviation(std8_to_wx(p_abbreviation));
		ret->set_description(wxString(""));
		ret->set_precision(p_precision);
		return ret;
	}
}  // end anonymous namespace



void
make_currencies
(	PhatbooksDatabaseConnection& dbc,
	vector<CommodityHandle>& vec
)
{
	JEWEL_ASSERT (vec.empty());  // precondition
	vec.reserve(200);
	// currency.inl contains repetitive code generated from a csv
	// file via a script.
	#include "make_currencies_inc.hpp"
	return;
}


vector<CommodityHandle>
make_currencies(PhatbooksDatabaseConnection& dbc)
{
	vector<CommodityHandle> ret;
	make_currencies(dbc, ret);
	return ret;
}



}  // namespace phatbooks
