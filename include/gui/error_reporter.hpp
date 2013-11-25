/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_error_reporter_hpp_24171430908223349
#define GUARD_error_reporter_hpp_24171430908223349

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <stdexcept>

namespace dcm
{
namespace gui
{

/**
 * @todo MEDIUM PRIORITY Document what this does.
 */
class ErrorReporter
{
public:

	ErrorReporter();
	ErrorReporter(ErrorReporter const& rhs) = delete;
	ErrorReporter(ErrorReporter&& rhs) = delete;
	ErrorReporter& operator=(ErrorReporter const& rhs) = delete;
	ErrorReporter& operator=(ErrorReporter&& rhs) = delete;
	virtual ~ErrorReporter();

	// client retains ownership of pointer
	void report(std::exception* p_exception = nullptr) const;

	void set_db_file_location
	(	boost::filesystem::path const& p_log_file_location
	);
	void set_backup_db_file_location
	(	boost::filesystem::path const& p_log_file_location
	);
	void set_log_file_location
	(	boost::filesystem::path const& p_log_file_location
	);

private:
	boost::optional<boost::filesystem::path> m_db_file_location;
	boost::optional<boost::filesystem::path> m_backup_db_file_location;
	boost::optional<boost::filesystem::path> m_log_file_location;

};  // class ErrorReporter

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_error_reporter_hpp_24171430908223349
