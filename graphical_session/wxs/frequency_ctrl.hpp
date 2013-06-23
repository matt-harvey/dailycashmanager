#ifndef GUARD_frequency_ctrl_hpp
#define GUARD_frequency_ctrl_hpp

#include <boost/noncopyable.hpp>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>

namespace phatbooks
{

// Begin forward declarations

class Frequency;

// End forward declarations

namespace gui
{

/**
 * Widget for the the user to select a Frequency.
 */
class FrequencyCtrl: public wxComboBox, private boost::noncopyable
{
public:

	FrequencyCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size
	);

	/**
	 * @todo Implement this.
	 */
	Frequency frequency() const;

};  // class FrequencyCtrl

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frequency_ctrl_hpp
