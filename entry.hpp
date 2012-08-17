#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

/** \file entry.hpp
 *
 * \brief Header file pertaining to Entry class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "date.hpp"
#include "journal.hpp"
#include "general_typedefs.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

/**
 * Class representing an accounting entry, i.e. a single line in an account.
 */
class Entry
{
public:

	/** Constructor.
	 * Does not throw (except possibly std::bad_alloc if the \c std::string
	 * allocation fails).
	 */
	Entry
	(	std::string const& p_account_name,	
		std::string const& p_comment = "",
		jewel::Decimal const& p_amount = jewel::Decimal("0")
	);

		
private:
	std::string m_account_name;
	std::string m_comment;
	jewel::Decimal m_amount;

};


}  // namespace phatbooks

#endif  // GUARD_entry_hpp
