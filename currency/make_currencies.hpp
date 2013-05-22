// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_make_currencies_hpp
#define GUARD_make_currencies_hpp

#include "commodity.hpp"
#include <vector>


namespace phatbooks
{

class PhatbooksDatabaseConnection;


/**
 * @returns a vector of almost all major currencies in the world, as
 * Commodity objects in a std::vector (where the Australian dollar, for
 * example, is a major currency but the Australian cent is not). Each
 * Commodity will have as its
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
 *
 * @todo The Japanese yen is officially divided into 100 sen; but a single
 * sen is of minuscule value, and it in everyday life, divisions smaller
 * than 1 yen are not used. Perhaps this is the case with other currencies.
 * Do we want to allow for this, and store some currencies in amounts
 * less precise than their official precision?
 */
std::vector<Commodity>
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
	std::vector<Commodity>& vec
);


}  // namespace phatbooks

#endif  // GUARD_make_currencies_hpp
