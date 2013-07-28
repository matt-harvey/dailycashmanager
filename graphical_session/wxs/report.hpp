#ifndef GUARD_report_hpp
#define GUARD_report_hpp

#include <boost/noncopyable.hpp>
#include <wx/scrolwin.h>

namespace phatbooks
{
namespace gui
{

class Report: public wxScrolledWindow, private boost::noncopyable
{
public:
	virtual ~Report();

protected:

private:

};  // class Report

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_report_hpp
