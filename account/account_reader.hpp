#ifndef GUARD_account_reader_hpp
#define GUARD_account_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class AccountImpl;
class PhatbooksDatabaseConnection;

typedef
	sqloxx::Reader<AccountImpl, PhatbooksDatabaseConnection>
	AccountReader;




}  // namespace phatbooks

#endif  // GUARD_account_reader_hpp
