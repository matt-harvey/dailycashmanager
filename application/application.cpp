#include "application.hpp"
#include "b_string.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/config.h>
#include <wx/string.h>
#include <cassert>
#include <cstdlib>
#include <string>

using boost::optional;
using jewel::value;
using std::getenv;
using std::string;
namespace filesystem = boost::filesystem;

namespace phatbooks
{


namespace
{
	wxString config_location_for_last_opened_file()
	{
		return wxString("/General/LastOpenedFile");
	}

}  // end anonymous namespace


BString
Application::application_name()
{
	return "Phatbooks";
}

BString
Application::filename_extension()
{
	return ".phat";
}

BString
Application::vendor_name()
{
	return "Phatbooks";
}

optional<filesystem::path>
Application::last_opened_file()
{
	optional<filesystem::path> ret;
	wxString wx_path;
	if (config().Read(config_location_for_last_opened_file(), &wx_path))
	{
		string const s_path = bstring_to_std8(wx_to_bstring(wx_path));
		assert (!s_path.empty());
		ret = filesystem::path(s_path);
	}
	else
	{
		assert (wx_path.IsEmpty());
		assert (!ret);
	}
	return ret;
}


void
Application::set_last_opened_file(filesystem::path const& p_path)
{
	// Assert precondition
	assert (filesystem::absolute(p_path) == p_path);

	string const s_path = p_path.string();
	wxString const wx_path = bstring_to_wx(std8_to_bstring(s_path));
	config().Write(config_location_for_last_opened_file(), wx_path);
	config().Flush();
	return;
}


optional<filesystem::path>
Application::default_directory()
{
	optional<filesystem::path> ret;
#	if JEWEL_ON_WINDOWS
	char const* win_home_drive = getenv("HOMEDRIVE");
	char const* win_home_path = getenv("HOMEPATH");
	if (win_home_drive && win_home_path)
	{
		string const home_str =
			string(win_home_drive) + string(win_home_path);
#	else
	if (char const* home_str = getenv("HOME"))  // assignment deliberate
	{
#	endif
		filesystem::path const home(home_str);
		if (filesystem::exists(filesystem::status(home)))
		{
			ret = filesystem::absolute(home);
		}
	}
	assert (!ret || (filesystem::absolute(value(ret)) == value(ret)));
	return ret;
}

wxConfig&
Application::config()
{
	static wxConfig* conf = 0;
	if (!conf)
	{
		conf = new wxConfig
		(	bstring_to_wx(application_name()),
			bstring_to_wx(vendor_name())
		);
	}
	assert (conf);
	return *conf;
}




}  // namespace phatbooks
