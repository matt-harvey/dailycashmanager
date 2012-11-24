#ifndef GUARD_account_type_hpp
#define GUARD_account_type_hpp

namespace phatbooks
{
namespace account_type
{


	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order. See setup_tables
		// method
		asset = 1,
		liability,
		equity,
		revenue,
		expense,
		pure_envelope
	};


}  // namespace account_type
}  // namespace phatbooks

#endif  // GUARD_account_type_hpp


