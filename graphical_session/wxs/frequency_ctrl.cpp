#include "frequency_ctrl.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using std::vector;

namespace phatbooks
{
namespace gui
{


// Anonymous namespace
namespace
{
	vector<Frequency> const& available_frequencies()
	{
		static vector<Frequency> ret;
		if (ret.empty())
		{
			ret.reserve(14);
			ret.push_back(Frequency(1, interval_type::days));
			ret.push_back(Frequency(1, interval_type::weeks));
			ret.push_back(Frequency(2, interval_type::weeks));
			ret.push_back(Frequency(4, interval_type::weeks));
			ret.push_back(Frequency(1, interval_type::months));
			ret.push_back(Frequency(2, interval_type::months));
			ret.push_back(Frequency(3, interval_type::months));
			ret.push_back(Frequency(4, interval_type::months));
			ret.push_back(Frequency(6, interval_type::months));
			ret.push_back(Frequency(1, interval_type::month_ends));
			ret.push_back(Frequency(2, interval_type::month_ends));
			ret.push_back(Frequency(3, interval_type::month_ends));
			ret.push_back(Frequency(4, interval_type::month_ends));
			ret.push_back(Frequency(6, interval_type::month_ends));
		}
		assert (!ret.empty());
		return ret;
	}

}  // end anonymous namespace


FrequencyCtrl::FrequencyCtrl
(	wxWindow* p_parent,
	wxWindowID p_id,
	wxSize const& p_size
):
	wxComboBox
	(	p_parent,
		p_id,
		wxString("Once off"),  // WARNING This isn't a Frequency!
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY
	)
{
	wxArrayString choices;
	choices.Add(wxString("One off"));
	vector<Frequency>::const_iterator it = available_frequencies().begin();
	vector<Frequency>::const_iterator const end =
		available_frequencies().end();
	for ( ; it != end; ++it)
	{
		Append(std8_to_wx(frequency_description(*it, "every")));
	}
}
	
	


}  // namespace gui
}  // namespace phatbooks
