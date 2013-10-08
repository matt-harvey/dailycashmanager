// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "application.hpp"
#include "string_conv.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/on_windows.hpp>
#include <jewel/optional.hpp>
#include <wx/config.h>
#include <wx/string.h>
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


wxString
Application::application_name()
{
	return wxString("Phatbooks");
}

wxString
Application::filename_extension()
{
	return wxString(".phat");
}

wxString
Application::vendor_name()
{
	return wxString("Phatbooks");
}

optional<filesystem::path>
Application::last_opened_file()
{
	optional<filesystem::path> ret;
	wxString wx_path;
	if (config().Read(config_location_for_last_opened_file(), &wx_path))
	{
		string const s_path = wx_to_std8(wx_path);
		JEWEL_ASSERT (!s_path.empty());
		ret = filesystem::path(s_path);
	}
	else
	{
		JEWEL_ASSERT (wx_path.IsEmpty());
		JEWEL_ASSERT (!ret);
	}
	return ret;
}


void
Application::set_last_opened_file(filesystem::path const& p_path)
{
	// Assert precondition
	JEWEL_ASSERT (filesystem::absolute(p_path) == p_path);

	string const s_path(p_path.string());
	wxString const wx_path(std8_to_wx(s_path));
	config().Write(config_location_for_last_opened_file(), wx_path);
	config().Flush();
	return;
}


optional<filesystem::path>
Application::default_directory()
{
	optional<filesystem::path> ret;
#  ifdef JEWEL_ON_WINDOWS
	char const* win_home_drive = getenv("HOMEDRIVE");
	char const* win_home_path = getenv("HOMEPATH");
	if (win_home_drive && win_home_path)
	{
		string const home_str =
			string(win_home_drive) + string(win_home_path);
#  else
	if (char const* home_str = getenv("HOME"))  // assignment deliberate
	{
#  endif
		filesystem::path const home(home_str);
		if (filesystem::exists(filesystem::status(home)))
		{
			ret = filesystem::absolute(home);
		}
	}
	JEWEL_ASSERT (!ret || (filesystem::absolute(value(ret)) == value(ret)));
	return ret;
}

wxConfig&
Application::config()
{
	static wxConfig* conf = 0;
	if (!conf)
	{
		conf = new wxConfig(application_name(), vendor_name());
	}
	JEWEL_ASSERT (conf);
	return *conf;
}




}  // namespace phatbooks
