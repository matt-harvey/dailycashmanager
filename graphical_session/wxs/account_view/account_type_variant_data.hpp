#ifndef GUARD_account_type_variant_data_hpp
#define GUARD_account_type_variant_data_hpp

#include "account_type.hpp"
#include <wx/string.h>
#include <wx/variant.h>
#include <iosfwd>

namespace phatbooks
{
namespace gui
{

/**
 * Derived from wxVariantData. This is to enable account_type::AccountType
 * data to be stored in a wxVariant.
 */
class AccountTypeVariantData: public wxVariantData
{
public:

	AccountTypeVariantData(account_type::AccountType p_account_type);

	/*
	account_type::AccountType account_type() const;
	*/

	/**
	 * Define function inherited as pure virtual from wxVariantData.
	 */
	bool Eq(wxVariantData& data) const;

	/**
	 * Like GetType (below) but is a static function.
	 */
	static wxString GetTypeStatic();

	/**
	 * Define function inherited as pure virtual from wxVariantData.
	 */
	wxString GetType() const;

	/**
	 * Define function inherited as pure virtual from wxVariantData.
	 */
	bool Read(std::istream& stream);

	/**
	 * Define function inherited as pure virtual from wxVariantData.
	 */
	bool Read(wxString& string);

	/**
	 * Define function inherited as pure virtual from wxVariantData.
	 */
	bool Write(std::ostream& stream) const;

	/**
	 * Define function inherited as pure virtual from wxVariantData.
	 *
	 * @param string should be empty when passed in.
	 */
	bool Write(wxString& string) const;


private:
	account_type::AccountType m_account_type;

};  // AccountTypeVariantData


}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_account_type_variant_data_hpp
