#ifndef GUARD_error_reporter_hpp_24171430908223349
#define GUARD_error_reporter_hpp_24171430908223349

#include <stdexcept>

namespace phatbooks
{
namespace gui
{

/**
 * @todo MEDIUM PRIORITY Document what this does.
 *
 * @todo LOW PRIORITY Is there any reason for this to be a class rather
 * than just a free-standing function?
 */
class ErrorReporter
{
public:

	// client retains ownership of pointer
	explicit ErrorReporter(std::exception* p_exception = nullptr);

	ErrorReporter(ErrorReporter const& rhs) = delete;
	ErrorReporter(ErrorReporter&& rhs) = delete;
	ErrorReporter& operator=(ErrorReporter const& rhs) = delete;
	ErrorReporter& operator=(ErrorReporter&& rhs) = delete;
	virtual ~ErrorReporter();

	void report() const;

private:
	std::exception* m_exception;

};  // class ErrorReporter

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_error_reporter_hpp_24171430908223349
