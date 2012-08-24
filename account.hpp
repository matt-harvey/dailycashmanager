#ifndef GUARD_account_hpp
#define GUARD_account_hpp

/** \file account.hpp
 *
 * \brief Header file pertaining to Account class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include "general_typedefs.hpp"
#include <string>
#include <map>

namespace phatbooks
{

class Account
{
public:


	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order
		revenue = 1,
		expense,
		asset,
		liability,
		pure_envelope
	};

	/**
	 * Constructor
	 * Does not throw
	 */
	Account
	(	std::string p_name,
		std::string p_commodity_abbreviation,
		AccountType p_account_type = asset,
		std::string p_description = ""
	);

	// Default copy contructor is fine.
	// Default assignment is fine.
	// Default destructor is fine.

	/** Returns name of account.
	 */
	std::string name() const;

	/** Returns abbreviation of native commodity of this account.
	 */
	std::string commodity_abbreviation() const;

	/** Returns AccountType of account.
	 */
	AccountType account_type() const;

	/** Returns description of account.
	 */
	std::string description() const;

	
private:

	std::string m_name;

	// native commodity or currency of Account
	std::string m_commodity_abbreviation;

	AccountType m_account_type;

	std::string m_description;

};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
