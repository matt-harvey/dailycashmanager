#include "frequency_ctrl.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include <boost/optional.hpp>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using boost::optional;
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

	wxString once_off_string()
	{	
		return wxString("Record once only, on");
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
		once_off_string(),
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY
	)
{
	Append(once_off_string());
	vector<Frequency>::const_iterator it = available_frequencies().begin();
	vector<Frequency>::const_iterator const end =
		available_frequencies().end();
	for ( ; it != end; ++it)
	{
		wxString wxs = wxString("Record ");
		wxs += std8_to_wx(frequency_description(*it, "every"));
		wxs += ", starting";
		Append(wxs);
	}
	SetSelection(0);
}

optional<Frequency>
FrequencyCtrl::frequency() const
{
	optional<Frequency> ret;
	vector<Frequency>::size_type const selection = GetSelection();
	if (selection == 0)
	{
		assert (GetValue() == once_off_string());
		assert (!ret);
		return ret;
	}
	assert (selection > 0);
	ret = available_frequencies()[selection - 1];
	return ret;
}

	


}  // namespace gui
}  // namespace phatbooks
