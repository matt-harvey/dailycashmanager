#ifndef GUARD_account_data_hpp_2769346448019774
#define GUARD_account_data_hpp_2769346448019774

#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include "visibility.hpp"
#include <boost/optional.hpp>
#include <wx/string.h>

namespace phatbooks
{

struct Account::AccountData
{
	boost::optional<wxString> name;
	boost::optional<Commodity> commodity;
	boost::optional<AccountType> account_type;
	boost::optional<wxString> description;
	boost::optional<Visibility> visibility;
};

}  // namespace phatbooks

#endif  // GUARD_account_data_hpp_2769346448019774
