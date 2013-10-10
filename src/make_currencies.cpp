/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

namespace phatbooks
{

namespace
{
	Handle<Commodity> make_currency
	(	PhatbooksDatabaseConnection& p_database_connection,
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
(	PhatbooksDatabaseConnection& dbc,
	vector<Handle<Commodity> >& vec
)
{
	JEWEL_ASSERT (vec.empty());  // precondition
	vec.reserve(200);
	// currency.inl contains repetitive code generated from a csv
	// file via a script.
	#include "make_currencies_inc.hpp"
	return;
}


vector<Handle<Commodity> >
make_currencies(PhatbooksDatabaseConnection& dbc)
{
	vector<Handle<Commodity> > ret;
	make_currencies(dbc, ret);
	return ret;
}



}  // namespace phatbooks
