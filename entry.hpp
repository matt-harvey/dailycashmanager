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
	(	boost::shared_ptr<Journal> p_journal,
		std::string const& p_comment = "",
		jewel::Decimal const& p_amount = jewel::Decimal("0"),
		bool p_is_actual = true
	);

		
private:
	boost::shared_ptr<Journal> m_journal;
	std::string m_comment;
	jewel::Decimal m_amount;
	bool m_is_actual;

};


}  // namespace phatbooks

#endif  // GUARD_entry_hpp
