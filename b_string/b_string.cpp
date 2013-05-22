// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "b_string.hpp"
#include <boost/static_assert.hpp>
#include <wx/string.h>
#include <string>

using std::string;


namespace phatbooks
{



std::string
bstring_to_std8(BString const& bs)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return wx_to_std8(bs);
#	else
		return bs;
#	endif
}


BString
std8_to_bstring(std::string const& s)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return std8_to_wx(s);	
#	else
		return s;
#	endif
}


wxString
bstring_to_wx(BString const& bs)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return bs;
#	else
		return std8_to_wx(bs);
#	endif
}


BString
wx_to_bstring(wxString const& wxs)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return wxs;
#	else
		return wx_to_std8(wxs);
#	endif
}


wxString
std8_to_wx(string const& s)
{
	return wxString::FromUTF8(s.c_str());
}


string
wx_to_std8(wxString const& wxs)
{
	return string(wxs.utf8_str());
}


BString
to_lower(BString const& bs)
{
	BOOST_STATIC_ASSERT((PHATBOOKS_USING_WX_STRING_AS_B_STRING));
	return bs.Lower();
}


}  // namespace phatbooks
