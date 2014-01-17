/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "make_currencies.hpp"
#include "commodity.hpp"
#include "string_conv.hpp"
#include <jewel/assert.hpp>
#include <sqloxx/handle.hpp>
#include <wx/string.h>
#include <string>
#include <vector>

using sqloxx::Handle;
using std::string;
using std::vector;

namespace dcm
{

namespace
{
	Handle<Commodity> make_currency
	(	DcmDatabaseConnection& p_database_connection,
		wxString const& p_name,  // So we can accept wide string literals
		string const& p_abbreviation,
		int p_precision
	)
	{
		Handle<Commodity> const ret(p_database_connection);
		ret->set_name(p_name);
		ret->set_abbreviation(std8_to_wx(p_abbreviation));
		ret->set_description(wxString(""));
		ret->set_precision(p_precision);
		return ret;
	}
}  // end anonymous namespace



void
make_currencies
(	DcmDatabaseConnection& dbc,
	vector<Handle<Commodity> >& vec
)
{
	JEWEL_ASSERT (vec.empty());  // precondition
	vec.reserve(200);
	// contains repetitive code generated from a csv
	// file via a script.
	#include DCM_CURRENCIES_INCLUDE_FILE
	return;
}


vector<Handle<Commodity> >
make_currencies(DcmDatabaseConnection& dbc)
{
	vector<Handle<Commodity> > ret;
	make_currencies(dbc, ret);
	return ret;
}



}  // namespace dcm
