#ifndef GUARD_entry_hpp
#define GUARD_entry_hpp

#include <boost/shared_ptr.hpp>

namespace phatbooks
{

class Entry
{
public:
	static void setup_tables(PhatbooksDatabaseConnection& dbc);

	explicit Entry
	(	boost::shared_ptr<PhatbooksDatabaseConnection> const&
			 p_database_connection,
	);

	Entry
	(	boos





};


}  // namespace phatbooks


#endif  // GUARD_entry_hpp
