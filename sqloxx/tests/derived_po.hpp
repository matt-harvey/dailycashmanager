#ifndef GUARD_derived_po_hpp
#define GUARD_derived_po_hpp

// Hide from Doxygen
/// @cond

#include "sqloxx/persistent_object.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace sqloxx
{
namespace tests
{

// Dummy class inheriting from PersistentObject, for the purpose
// of testing PersistentObject class.
class DerivedPO: public PersistentObject<DerivedPO>
{

public:
	typedef int Id;
	static void setup_tables(DatabaseConnection& dbc);
	explicit DerivedPO(boost::shared_ptr<DatabaseConnection> p_dbc);
	DerivedPO(boost::shared_ptr<DatabaseConnection> p_dbc, Id p_id);
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


}  // namespace tests
}  // namespace sqloxx

/// @endcond
// End hiding from Doxygen

#endif  // GUARD_derived_po_hpp
