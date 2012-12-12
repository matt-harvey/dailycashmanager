#ifndef GUARD_account_reader_hpp
#define GUARD_account_reader_hpp

#include "sqloxx/reader.hpp"

namespace phatbooks
{

class AccountImpl;
class PhatbooksDatabaseConnection;

class AccountReader:
	public sqloxx::Reader<AccountImpl, PhatbooksDatabaseConnection>
{
public:
	explicit AccountReader
	(	PhatbooksDatabaseConnection& p_database_connection
	);
};

}  // namespace phatbooks

#endif  // GUARD_account_reader_hpp
