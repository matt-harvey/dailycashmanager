#ifndef GUARD_b_string_hpp
#define GUARD_b_string_hpp

#include <wx/string.h>
#include <string>



/**
 * BString is the type of string used at the "business layer" of
 * Phatbooks. It will probably always just be a typedef for some
 * other type of string. We have conversion functions here to
 * convert from BString to UTF-8 std::string and back, and to convert
 * from BString to wxString and back. These may or may not be
 * no-ops, but we should use them in the rest of the code
 * regardless. The idea is that if we ever want to change the
 * type of BString, we should only have to change this file.
 *
 * @todo Testing and documentation.
 */


namespace phatbooks
{

#define PHATBOOKS_USING_WX_STRING_AS_B_STRING 0

#if PHATBOOKS_USING_WX_STRING_AS_B_STRING
	typedef wxString BString;
#else
	typedef std::string BString;
#endif


std::string bstring_to_std8(BString const& bs);
BString std8_to_bstring(std::string const& s);

wxString bstring_to_wx(BString const& wxs);
BString wx_to_bstring(wxString const& wxs);


// IMPLEMENTATIONS

inline
std::string
bstring_to_std8(BString const& bs)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return std::string(bs.utf8_str());	
#	else
		return bs;
#	endif
}

inline
BString
std8_to_bstring(std::string const& s)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return wxString::FromUTF8(s.c_str());	
#	else
		return s;
#	endif
}

inline
wxString
bstring_to_wx(BString const& bs)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return bs;
#	else
		return wxString::FromUTF8(bs.c_str());
#	endif
}

inline
BString
wx_to_bstring(wxString const& wxs)
{
#	if PHATBOOKS_USING_WX_STRING_AS_B_STRING
		return wxs;
#	else
		return std::string(wxs.utf8_str());
#	endif
}


}  // namespace phatbooks



#endif  // GUARD_b_string_hpp
