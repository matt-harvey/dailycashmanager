// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_account_handle_hpp_7530966385980586
#define GUARD_account_handle_hpp_7530966385980586

#include "account.hpp"
#include <sqloxx/handle.hpp>

namespace phatbooks
{

class PhatbooksDatabaseConnection;

typedef sqloxx::Handle<Account> AccountHandle;


}  // namespace phatbooks

#endif  // GUARD_account_handle_hpp_7530966385980586
