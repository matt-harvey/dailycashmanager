#include "application.hpp"
#include "b_string.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <wx/config.h>
#include <wx/string.h>
#include <cassert>
#include <string>

using boost::optional;
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
	string const s_path = p_path.string();
	wxString const wx_path = bstring_to_wx(std8_to_bstring(s_path));
	config().Write(config_location_for_last_opened_file(), wx_path);
	config().Flush();
	return;
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
