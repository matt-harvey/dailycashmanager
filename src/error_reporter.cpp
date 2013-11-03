/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "gui/error_reporter.hpp"
#include "app.hpp"
#include "string_conv.hpp"
#include <boost/filesystem.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <wx/log.h>
#include <wx/string.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using jewel::Log;
using std::endl;
using std::ostringstream;
using std::string;
using std::strlen;
namespace filesystem = boost::filesystem;

namespace phatbooks
{
namespace gui
{

ErrorReporter::ErrorReporter()
{
	JEWEL_ASSERT (m_log_file_location.empty());
}

ErrorReporter::~ErrorReporter() = default;

void
ErrorReporter::report(std::exception* p_exception) const
{
	ostringstream oss;
	oss << "There has been an error. Details are as follows.\n\n";
	std::exception* e_ptr = nullptr;
	if ((e_ptr = dynamic_cast<jewel::Exception*>(p_exception)))
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
	else if ((e_ptr = dynamic_cast<std::exception*>(p_exception)))
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
		oss << "Unknown exception";
	}
	if (!m_log_file_location.empty())
	{
		oss << "A detailed session log is located in the following file:\n"
		    << m_log_file_location
		    << "\n\nIt is recommended to send a copy of the log file to the "
		    << "application developer for troubleshooting."
			<< endl;
	}
	// TODO HIGH PRIORITY Enable this user to click "Yes" to "take action", or
	// "No" to ignore. "Taking action" could mean either (a) saving a copy
	// of the error log for later manual emailing to the developer, or
	// (b) automatically sending the error log to some server via http
	// (or something). Use the third party library wxCrashReport? Also,
	// tell the user about the existence of the automatically-generated
	// backup file (after first verifying its existence), perhaps giving them
	// the option to revert to this earlier file at the click of a mouse.
	wxLogError(std8_to_wx(oss.str()));
	return;
}

void
ErrorReporter::set_log_file_location(string const& p_log_file_location)
{
	m_log_file_location = p_log_file_location;
	return;
}

}  // namespace gui
}  // namespace phatbooks
