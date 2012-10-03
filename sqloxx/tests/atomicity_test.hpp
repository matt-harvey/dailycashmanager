#ifndef GUARD_atomicity_test_hpp
#define GUARD_atomicity_test_hpp

#include "sqloxx/shared_sql_statement.hpp"
#include <string>

using std::string;

/**
 * @file atomicity_test_hpp
 *
 * This file declares a function that is designed to create a database
 * file, start a transaction by calling
 * DatabaseConnection::begin_transaction, and then crash
 * before being able to complete the transaction. The idea
 * is that an external script will then examine the database
 * after the crash, to ensure transaction atomicity was upheld.
 */

namespace sqloxx
{

// Forward declaration
class DatabaseConnection;



namespace tests
{


int
do_atomicity_test();

void
setup_atomicity_test(DatabaseConnection& dbc);

int
inspect_database_for_atomicity(std::string const& db_filename);


}  // namespace tests
}  // namespace sqloxx




#endif  // GUARD_atomicity_test_hpp
