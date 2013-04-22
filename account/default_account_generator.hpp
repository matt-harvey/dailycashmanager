#ifndef GUARD_default_account_generator_hpp
#define GUARD_default_account_generator_hpp

#include "account.hpp"
#include "b_string.hpp"
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
	 * 
	 * The returned Accounts do not have their commodity initialized.
	 *
	 * @todo Ensure this does not cause existing Accounts to be overwritten.
	 */
	std::vector<Account>& accounts();

	/**
	 * Save all the Accounts currently stored in the DefaultAccountGenerator.
	 * This should be called after the user has been given the opportunity
	 * to accept or reject the default Accounts.
	 */
	void save_accounts();

	/**
	 * @returns the Account with p_name as its name(), from among those
	 * currently stored in the DefaultAccountGenerator. There is guaranteed
	 * to be no more than one Account stored with a given name.
	 *
	 * @throws DefaultAccountGeneratorException if there is no Account with
	 * this name in the DefaultAccountGenerator.
	 *
	 * @todo Enforce the abovementioned guarantee.
	 */
	Account get_account_named(BString const& p_name) const;

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
