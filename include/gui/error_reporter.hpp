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

#ifndef GUARD_error_reporter_hpp_24171430908223349
#define GUARD_error_reporter_hpp_24171430908223349

#include <stdexcept>
#include <string>

namespace phatbooks
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

	void set_db_file_location(std::string const& p_log_file_location);
	void set_backup_db_file_location(std::string const& p_log_file_location);
	void set_log_file_location(std::string const& p_log_file_location);

private:
	std::string m_db_file_location;
	std::string m_backup_db_file_location;
	std::string m_log_file_location;

};  // class ErrorReporter

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_error_reporter_hpp_24171430908223349
