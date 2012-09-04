#include "account_base.hpp"
#include <cassert>
#include <string>
#include <vector>

/** \file account_base.cpp
 *
 * \brief Source file for code pertaining to AccountBase class.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */


using std::string;
using std::vector;

namespace phatbooks
{

vector<string>
AccountBase::account_type_names()
{
	static bool calculated_already = false;
	static vector<string> ret;
	while (!calculated_already)
	{
		ret.push_back("Asset");
		ret.push_back("Liability");
		ret.push_back("Revenue category");
		ret.push_back("Expense category");
		ret.push_back("Pure envelope");
		calculated_already = true;
	}
	return ret;
}


}  // namespace phatbooks
