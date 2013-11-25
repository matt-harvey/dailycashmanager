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

#ifndef GUARD_make_currencies_hpp_7990285213100857
#define GUARD_make_currencies_hpp_7990285213100857

#include <sqloxx/handle_fwd.hpp>
#include <vector>


namespace dcm
{

class Commodity;
class DcmDatabaseConnection;


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
make_currencies(DcmDatabaseConnection& p_database_connection);


/**
 * Behaves like the make_currencies(DcmDatabaseConnection&),
 * but instead of returning a vector, it populates the vector
 * passed to \e vec.
 *
 * @param vec the vector which the function will populate with
 * Commodities. \e vec should be empty when passed to the function.
 */
void
make_currencies
(	DcmDatabaseConnection& p_database_connection,	
	std::vector<sqloxx::Handle<Commodity> >& vec
);


}  // namespace dcm

#endif  // GUARD_make_currencies_hpp_7990285213100857
