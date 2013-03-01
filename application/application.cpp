#include "application.hpp"
#include "b_string.hpp"


namespace phatbooks
{

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

}  // namespace phatbooks
