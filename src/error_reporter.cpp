#include "gui/error_reporter.hpp"
#include "string_conv.hpp"
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <wx/log.h>
#include <wx/string.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

using jewel::Log;
using std::endl;
using std::ostringstream;
using std::strlen;

namespace phatbooks
{
namespace gui
{

ErrorReporter::ErrorReporter(std::exception* p_exception):
	m_exception(p_exception)
{
}

ErrorReporter::~ErrorReporter() = default;

void
ErrorReporter::report() const
{
	ostringstream oss;
	std::exception* e_ptr = nullptr;
	if ((e_ptr = dynamic_cast<jewel::Exception*>(m_exception)))
	{
		JEWEL_LOG_MESSAGE(Log::error, "jewel::Exception.");
		auto const je_ptr = dynamic_cast<jewel::Exception*>(e_ptr);
		JEWEL_ASSERT (je_ptr);
		JEWEL_LOG_VALUE(Log::error, *je_ptr);
		oss << *je_ptr << endl;
		if (strlen(je_ptr->type()) == 0)
		{
			JEWEL_LOG_VALUE(Log::error, typeid(*e_ptr).name());
			oss << "typeid(*e_ptr).name(): "
			    << typeid(*e_ptr).name()
				<< endl;
		}
	}
	else if ((e_ptr = dynamic_cast<std::exception*>(m_exception)))
	{
		JEWEL_LOG_MESSAGE(Log::error, "std::exception");
		JEWEL_LOG_VALUE(Log::error, typeid(*e_ptr).name());
		JEWEL_LOG_VALUE(Log::error, e_ptr->what());
		oss << "EXCEPTION:" << endl;
		oss << "typeid(*e_ptr).name(): " << typeid(*e_ptr).name() << endl;
		oss << "e_ptr->what(): " << e_ptr->what() << endl;
	}
	else
	{
		JEWEL_LOG_MESSAGE(Log::error, "Unknown exception.");
		wxLogError("Unknown exception.");
		oss << "Unkown exception";
	}
	wxLogError(std8_to_wx(oss.str()));
	return;
}

}  // namespace gui
}  // namespace phatbooks
