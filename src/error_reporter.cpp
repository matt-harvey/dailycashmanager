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
#include "backup.hpp"
#include "date.hpp"
#include "phatbooks_exceptions.hpp"
#include "string_conv.hpp"
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using boost::optional;
using jewel::Log;
using std::bad_alloc;
using std::endl;
using std::ostringstream;
using std::string;
using std::strlen;
namespace filesystem = boost::filesystem;
namespace posix_time = boost::posix_time;

namespace phatbooks
{
namespace gui
{

ErrorReporter::ErrorReporter()
{
	JEWEL_ASSERT (!m_db_file_location);
	JEWEL_ASSERT (!m_backup_db_file_location);
	JEWEL_ASSERT (!m_log_file_location);
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
		oss << "Unknown exception" << endl;
	}
	if (m_backup_db_file_location && m_db_file_location)
	{
		oss << "\nFor your information a backup copy of your file \""
		    << m_db_file_location->filename()
			<< "\" was taken at the beginning of the current session, "
			<< "and has now been saved to the following location:\n"
			<< *m_backup_db_file_location
			<< endl;
	}	
	if (m_log_file_location)
	{
		filesystem::path log_filepath = *m_log_file_location;	
		if (m_db_file_location)
		{
			// Take a copy of the log file named after the current date and
			// time and save in the same directory of the database file.
			try
			{
				JEWEL_ASSERT
				(	filesystem::absolute(log_filepath) ==
					log_filepath
				);
				log_filepath = make_backup
				(	log_filepath,
					m_db_file_location->parent_path()
				);
			}
			catch (UniqueNameException&)
			{
				// do nothing
			}
			catch (filesystem::filesystem_error&)
			{
				// do nothing
			}
			catch (bad_alloc&)
			{
				// do nothing
			}
			JEWEL_LOG_TRACE();
		}
		// log_filepath now corresponds to the copy of the log that we just
		// made, or, if we couldn't make that copy, to the original logfile
		// filepath.
		oss << "\n\nA detailed session log has been saved here:\n"
		    << log_filepath
		    << "\n\nIt is recommended to send a copy of the log file to the "
		    << "application developer for troubleshooting.\n"
		    << endl;

	}
	// TODO MEDIUM PRIORITY Enable this user to send a copy
	// of the error log to the developer at the click of a mouse.

	// NOTE wxLogError doesn't fit the message.
	wxMessageBox(std8_to_wx(oss.str()), "Error", wxICON_EXCLAMATION | wxOK);
	return;
}

void
ErrorReporter::set_db_file_location
(	filesystem::path const& p_db_file_location
)
{
	m_db_file_location = p_db_file_location;
	return;
}

void
ErrorReporter::set_backup_db_file_location
(	filesystem::path const& p_backup_db_file_location
)
{
	m_backup_db_file_location = p_backup_db_file_location;
	return;
}

void
ErrorReporter::set_log_file_location
(	filesystem::path const& p_log_file_location
)
{
	m_log_file_location = p_log_file_location;
	return;
}

}  // namespace gui
}  // namespace phatbooks
