#ifndef GUARD_derived_po_hpp
#define GUARD_derived_po_hpp

// Hide from Doxygen
/// @cond

#include "sqloxx/database_connection.hpp"
#include "sqloxx/identity_map.hpp"
#include "sqloxx/persistent_object.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace sqloxx
{
namespace tests
{

class DerivedDatabaseConnection;

// Dummy class inheriting from PersistentObject, for the purpose
// of testing PersistentObject class.
class DerivedPO:
	public PersistentObject<DerivedPO, DerivedDatabaseConnection>
{

public:
	typedef sqloxx::Id Id;
	typedef PersistentObject<DerivedPO, DerivedDatabaseConnection>
		PersistentObject;
	typedef sqloxx::IdentityMap<DerivedPO, DerivedDatabaseConnection>
		IdentityMap;
	static void setup_tables(DerivedDatabaseConnection& dbc);
	explicit DerivedPO(IdentityMap& p_identity_map);
	DerivedPO(IdentityMap& p_identity_map, Id p_id);
	int x();
	double y();
	void set_x(int p_x);
	void set_y(double p_y);
	// Default destructor is OK.
	
	// To test protected functions of PersistentObject. Returns number
	// of failing checks in test.
	static int self_test();	

	static std::string primary_table_name();
protected:
	DerivedPO(DerivedPO const& rhs);

private:
	void do_load();
	// Uses default version of do_calculate_prospective_key
	void do_save_existing();
	void do_save_new();
	int m_x;
	double m_y;
};


// Dummy class derived from DatabaseConnection, to provide IdentityMap<DerivedPO>
class DerivedDatabaseConnection: public DatabaseConnection
{
public:

	DerivedDatabaseConnection();

	typedef sqloxx::IdentityMap<DerivedPO, DerivedDatabaseConnection>
		IdentityMap;
	IdentityMap& derived_po_map()
	{
		return m_derived_po_map;
	}

	template <typename T>
	IdentityMap& identity_map();

private:

	IdentityMap m_derived_po_map;
};


template <>
inline
IdentityMap<DerivedPO, DerivedDatabaseConnection>&
DerivedDatabaseConnection::identity_map<DerivedPO>()
{
	return m_derived_po_map;
}


}  // namespace tests


}  // namespace sqloxx

/// @endcond
// End hiding from Doxygen

#endif  // GUARD_derived_po_hpp
