#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

#include "date.hpp"
#include "journal.hpp"
#include "general_typedefs.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

namespace phatbooks
{

class Entry
{
public:

	/** Constructor.
	 * Does not throw.
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
