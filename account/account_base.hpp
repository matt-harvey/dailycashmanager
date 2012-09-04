#ifndef GUARD_account_base_hpp
#define GUARD_account_base_hpp

/** \file account_base.hpp
 *
 * \brief Header file pertaining to AccountBase class.
 *
 * \author Matthew Harvey
 * \date 04 Sep 2012
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


#include <string>
#include <vector>

namespace phatbooks
{

/**
 * Abstract base class providing interface.
 */
class AccountBase
{
public:


	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order - see
		// account_storage_manager.hpp
		revenue = 1,
		expense,
		asset,
		liability,
		pure_envelope
	};

	/**
	 * Returns a vector of account type names, corresponding to the
	 * AccountType enumerations, and in the same order.
	 */
	static std::vector<std::string> account_type_names();


	/** Returns name of account.
	 */
	virtual std::string name() const = 0;

	/** Returns abbreviation of native commodity of this account.
	 */
	virtual std::string commodity_abbreviation() const = 0;

	/** Returns AccountType of account.
	 */
	virtual AccountType account_type() const = 0;

	/** Returns description of account.
	 */
	virtual std::string description() const = 0;

	

};


}  // namespace phatbooks


#endif  // GUARD_account_hpp
