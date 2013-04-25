#include "account_type_variant_data.hpp"
#include "account_type.hpp"
#include <wx/string.h>
#include <istream>
#include <ostream>
#include <string>
#include <typeinfo>

using std::istream;
using std::ostream;
using std::string;

namespace phatbooks
{
namespace gui
{

AccountTypeVariantData::AccountTypeVariantData
(	account_type::AccountType p_account_type
):
	m_account_type(p_account_type)
{
}

bool
AccountTypeVariantData::Eq(wxVariantData& data) const
{
	assert (typeid(*this) == typeid(data));
	AccountTypeVariantData& adata =
		dynamic_cast<AccountTypeVariantData&>(data);
	return adata.m_account_type == m_account_type;
}

wxString
AccountTypeVariantData::GetType() const
{
	return wxString("phatbooks::account_type::AccountType");
}

bool
AccountTypeVariantData::Read(istream& stream)
{
	try
	{
		string s;
		stream >> s;
		m_account_type = string_to_account_type
		(	std8_to_bstring(s)
		);
		return static_cast<bool>(stream);
	}
	catch (...)
	{
		return false;
	}
}

bool
AccountTypeVariantData::Read(wxString& string)
{
	try
	{
		BString const bs = wx_to_bstring(string);
		m_account_type = string_to_account_type(bs);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool
AccountTypeVariantData::Write(ostream& stream) const
{
	try
	{
		stream << bstring_to_wx(account_type_to_string(m_account_type));
		return static_cast<bool>(stream);
	}
	catch (...)
	{
		return false;
	}
}

bool
AccountTypeVariantData::Write(wxString& string) const
{
	assert (string.IsEmpty());
	try
	{
		string = bstring_to_wx(account_type_to_string(m_account_type));
		return !string.IsEmpty();
	}
	catch (...)
	{
		return false;
	}
}




}  // namespace gui
}  // namespace phatbooks
