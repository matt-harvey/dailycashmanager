// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "frequency_ctrl.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/optional.hpp>
#include <jewel/optional.hpp>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using boost::optional;
using jewel::value;
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
			ret.push_back(Frequency(3, interval_type::weeks));
			ret.push_back(Frequency(4, interval_type::weeks));
			ret.push_back(Frequency(1, interval_type::months));
			ret.push_back(Frequency(2, interval_type::months));
			ret.push_back(Frequency(3, interval_type::months));
			ret.push_back(Frequency(4, interval_type::months));
			ret.push_back(Frequency(6, interval_type::months));
			ret.push_back(Frequency(12, interval_type::months));
			ret.push_back(Frequency(1, interval_type::month_ends));
			ret.push_back(Frequency(2, interval_type::month_ends));
			ret.push_back(Frequency(3, interval_type::month_ends));
			ret.push_back(Frequency(4, interval_type::month_ends));
			ret.push_back(Frequency(6, interval_type::month_ends));
			ret.push_back(Frequency(12, interval_type::month_ends));
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
	wxSize const& p_size,
	bool p_support_ordinary_journal,
	bool p_support_draft_journal
):
	wxComboBox
	(	p_parent,
		p_id,
		once_off_string(),
		wxDefaultPosition,
		p_size,
		wxArrayString(),
		wxCB_READONLY
	),
	m_support_ordinary_journal(p_support_ordinary_journal),
	m_support_draft_journal(p_support_draft_journal)
{
	assert (m_support_ordinary_journal || m_support_draft_journal);
	if (m_support_ordinary_journal)
	{
		Append(once_off_string());
	}
	if (m_support_draft_journal)
	{
		vector<Frequency>::const_iterator it =
			available_frequencies().begin();
		vector<Frequency>::const_iterator const end =
			available_frequencies().end();
		for ( ; it != end; ++it)
		{
			wxString wxs = wxString("Record ");
			wxs += std8_to_wx(frequency_description(*it, "every"));
			wxs += ", starting";
			Append(wxs);
		}
	}
	SetSelection(0);
}

optional<Frequency>
FrequencyCtrl::frequency() const
{
	optional<Frequency> ret;
	vector<Frequency>::size_type index = GetSelection();
	if (m_support_ordinary_journal)
	{
		if (index == 0)
		{
			assert (GetValue() == once_off_string());
			assert (!ret);
			return ret;
		}
		assert (index >= 1);
		index -= 1;
	}
	ret = available_frequencies()[index];
	return ret;
}

void
FrequencyCtrl::set_frequency(optional<Frequency> const& p_maybe_frequency)
{
	if (p_maybe_frequency)
	{
		if (!m_support_draft_journal)
		{
			throw InvalidFrequencyException
			(	"FrequencyCtrl does not support recurring transaction "
				"Frequencies."
			);
		}
		assert (m_support_draft_journal);
		Frequency const freq = value(p_maybe_frequency);
		vector<Frequency>::const_iterator it =
			available_frequencies().begin();
		vector<Frequency>::const_iterator const end =
			available_frequencies().end();
		vector<Frequency>::size_type i = (m_support_ordinary_journal? 1: 0);
		for ( ; it != end; ++it, ++i)
		{
			if (*it == freq)
			{
				SetSelection(i);
				return;
			}
		}
		assert (false);
	}
	assert (!p_maybe_frequency);
	if (!m_support_ordinary_journal)
	{
		throw InvalidFrequencyException
		(	"FrequencyCtrl does not support \"once-off\" selection."
		);
	}
	assert (m_support_ordinary_journal);
	SetSelection(0);
	return;
}

}  // namespace gui
}  // namespace phatbooks
