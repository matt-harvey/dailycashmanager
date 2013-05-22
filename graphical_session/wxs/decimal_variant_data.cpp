// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "decimal_variant_data.hpp"
#include "app.hpp"
#include "b_string.hpp"
#include "finformat.hpp"
#include "locale.hpp"
#include <jewel/decimal.hpp>
#include <wx/string.h>
#include <cassert>
#include <istream>
#include <ostream>
#include <string>
#include <typeinfo>

using jewel::Decimal;
using std::istream;
using std::ostream;
using std::string;

namespace phatbooks
{
namespace gui
{

DecimalVariantData::DecimalVariantData(Decimal const& p_decimal):
	m_decimal(p_decimal)
{
}

Decimal
DecimalVariantData::decimal() const
{
	return m_decimal;
}

bool
DecimalVariantData::Eq(wxVariantData& data) const
{
	assert (typeid(*this) == typeid(data));
	DecimalVariantData& dvdata =
		dynamic_cast<DecimalVariantData&>(data);
	return dvdata.m_decimal == m_decimal;
}

wxString
DecimalVariantData::GetTypeStatic()
{
	return wxString("jewel::Decimal");
}

wxString
DecimalVariantData::GetType() const
{
	return GetTypeStatic();
}

bool
DecimalVariantData::Read(std::istream& stream)
{
	try
	{	
		string s;
		stream >> s;
		wxString wxs = std8_to_wx(s);
		m_decimal = wx_to_decimal(wxs, locale());
		return static_cast<bool>(stream);
	}
	catch (...)
	{
		return false;
	}
}

bool
DecimalVariantData::Read(wxString& string)
{
	try
	{
		m_decimal = wx_to_decimal(string, locale());
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool
DecimalVariantData::Write(ostream& stream) const
{
	try
	{
		wxString wxs;
		wxs = finformat_wx(m_decimal, locale());
		stream << wx_to_std8(wxs);
		return static_cast<bool>(stream);
	}
	catch (...)
	{
		return false;
	}
}

bool
DecimalVariantData::Write(wxString& string) const
{
	assert (string.IsEmpty());
	try
	{
		string = finformat_wx(m_decimal, locale());
		return true;
	}
	catch (...)
	{
		return false;
	}
}


}  // namespace gui
}  // namespace phatbooks
