#ifndef GUARD_account_type_variant_data_hpp
#define GUARD_account_type_variant_data_hpp

#include "account_type.hpp"
#include <wx/variant.h>

namespace phatbooks
{
namespace gui
{

/**
 * Derived from wxVariantData. This is to enable account_type::AccountType
 * data to be stored in a wxVariant.
 */
class AccountTypeVariantData
{
public:
	AccountTypeVariantData(account_type::AccountType p_account_type);
	virtual bool Eq(wxVariantData& data) const;
	virtual wxString GetType() const;

	virtual bool Read(std::istream& stream);
	virtual bool Read(wxString& string);
	virtual bool Write(std::ostream& stream) const;

	/**
	 * string should be empty when passed in.
	 */
	virtual bool Write(wxString& string) const;


private:
	account_type::AccountType m_account_type;

};  // AccountTypeVariantData


}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_account_type_variant_data_hpp
