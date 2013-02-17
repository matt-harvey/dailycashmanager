#ifndef GUARD_account_type_hpp
#define GUARD_account_type_hpp

#include <wx/string.h>
#include <vector>

namespace phatbooks
{
namespace account_type
{


	enum AccountType
	{
		// enum order is significant, as the database contains
		// a table with primary keys in this order.
		// Other functions in this file also depend on
		// the exact size and order of this enumeration.
		asset = 1,
		liability,
		equity,
		revenue,
		expense,
		pure_envelope
	};



}  // namespace account_type


std::vector<wxString>
account_type_names();

wxString
account_type_to_string(account_type::AccountType p_account_type);

account_type::AccountType
string_to_account_type(wxString const& p_string);


}  // namespace phatbooks

#endif  // GUARD_account_type_hpp


