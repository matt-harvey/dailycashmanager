#ifndef GUARD_default_account_generator_hpp
#define GUARD_default_account_generator_hpp

#include "account.hpp"
#include <boost/noncopyable.hpp>
#include <vector>

namespace phatbooks
{

class PhatbooksDatabaseConnection;

/**
 * Class for managing the production of a default list of
 * Accounts to be presented to the user on first using
 * the application.
 *
 * @todo Make sure this is destroyed in the correct order on
 * application shutdown to avoid dangling reference to
 * PhatbooksDatabaseConnection. Would it be better
 * if we made m_phatbooks_database_connection a shared_ptr?
 */
class DefaultAccountGenerator:
	private boost::noncopyable
{
public:
	DefaultAccountGenerator
	(	PhatbooksDatabaseConnection& p_database_connection
	);

	~DefaultAccountGenerator();

	/**
	 * @returns a reference to a vector of default Accounts
	 * (which are memory-managed by the DefaultAccountGenerator).
	 * The vector is non-const because it is intended for client
	 * code to alter the vector in reponse to user's wishes,
	 * prior to calling save on those Accounts which the user
	 * wishes to keep.
	 *
	 * Note this does not include the budget balancing Account - since
	 * this is \e always present and cannot be opted-away by the user.
	 */
	std::vector<Account>& accounts();

private:
	
	/**
	 * Preconditions:
	 * m_accounts must have been allocated, but should be empty of elements.
	 */
	void initialize_default_accounts();

	std::vector<Account>* m_accounts;
	PhatbooksDatabaseConnection& m_database_connection;

};  // DefaultAccountGenerator

}  // namespace phatbooks

#endif  // GUARD_default_account_generator_hpp
