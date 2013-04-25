#ifndef GUARD_decimal_variant_data_hpp
#define GUARD_decimal_variant_data_hpp

#include <jewel/decimal.hpp>
#include <wx/string.h>
#include <wx/variant.h>
#include <iosfwd>

namespace phatbooks
{
namespace gui
{

/**
 * Derived from wxVariantData. This is to enable jewel::Decimal data
 * to be stored in a wxVariant.
 */
class DecimalVariantData: public wxVariantData
{
public:
	DecimalVariantData(jewel::Decimal const& p_decimal);

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
	 * string should be empty when passed in.
	 */
	bool Write(wxString& string) const;

private:
	jewel::Decimal m_decimal;

};  // DecimalVariantData

}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_decimal_variant_data_hpp
