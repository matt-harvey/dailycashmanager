#include "identity_manager.hpp"
#include "database_connection.hpp"

#include <boost/shared_ptr.hpp>
#include <vector>

using boost::shared_ptr;
using std::vector;

namespace sqloxx
{

IdentityManager::IdentityManager
(	shared_ptr<DatabaseConnection> p_database_connection,
	vector<int> const& record_map_identifiers
):
	m_database_connection(p_database_connection)
{
	for
	(	vector<int>::const_iterator it = record_map_identifiers.begin();
		it != record_map_identifiers.end();
		++it
	)
	{
		RecordMapPointer rmp(new RecordMap);
		m_master_map[*it] = rmp;
	}
}





}  // namespace sqloxx
