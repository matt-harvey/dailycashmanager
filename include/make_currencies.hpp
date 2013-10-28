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


#ifndef GUARD_make_currencies_hpp_7990285213100857
#define GUARD_make_currencies_hpp_7990285213100857

#include <sqloxx/handle_fwd.hpp>
#include <vector>


namespace phatbooks
{

class Commodity;
class PhatbooksDatabaseConnection;


/**
 * @returns a vector of almost all major currencies in the world, as
 * sqloxx::Handle<Commodity> objects in a std::vector (where the Australian
 * dollar, for example, is a major currency but the Australian cent is not).
 * Each Commodity will have as its
 * abbreviation the three-letter ISO symbol of the currency it
 * represents (e.g. "AUD"); will have its name initialized to the
 * name of the corresponding currency, in a form such as e.g.
 * "Australian dollar"; and will have its precision set in accordance
 * with the number of minor units in the corresponding currency. For
 * example, since the Australian dollar is divided into 100 cents, the
 * precision of the corresponding Commodity will be 2.
 *
 * None of the Commodities in the returned vector will have
 * multiplier_to_base() initialized.
 *
 * @param p_database_connection a connection to the database with
 * which the returned Commodities will be associated (but note, calling
 * this function will \e not cause the Commodities to be saved to the
 * database).
 */
std::vector<sqloxx::Handle<Commodity> >
make_currencies(PhatbooksDatabaseConnection& p_database_connection);


/**
 * Behaves like the make_currencies(PhatbooksDatabaseConnection&),
 * but instead of returning a vector, it populates the vector
 * passed to \e vec.
 *
 * @param vec the vector which the function will populate with
 * Commodities. \e vec should be empty when passed to the function.
 */
void
make_currencies
(	PhatbooksDatabaseConnection& p_database_connection,	
	std::vector<sqloxx::Handle<Commodity> >& vec
);


}  // namespace phatbooks

#endif  // GUARD_make_currencies_hpp_7990285213100857
