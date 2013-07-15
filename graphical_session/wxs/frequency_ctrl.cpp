// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "frequency_ctrl.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "phatbooks_database_connection.hpp"
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
	PhatbooksDatabaseConnection& p_database_connection,
	bool p_supports_ordinary_journal,
	bool p_supports_draft_journal
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
	m_database_connection(p_database_connection),
	m_supports_ordinary_journal(p_supports_ordinary_journal),
	m_supports_draft_journal(p_supports_draft_journal)
{
	assert (m_frequencies.empty());
	if (supports_ordinary_journal())
	{
		Append(once_off_string());
	}
	if (supports_draft_journal() || supports_budget_item())
	{
		vector<Frequency>::const_iterator it =
			available_frequencies().begin();
		vector<Frequency>::const_iterator const end =
			available_frequencies().end();
		if (supports_budget_item())
		{
			assert (!supports_draft_journal());
			for ( ; it != end; ++it)
			{
				if (m_database_connection.supports_budget_frequency(*it))
				{
					Append(std8_to_wx(frequency_description(*it)));
					m_frequencies.push_back(*it);
				}
			}
		}
		else
		{
			assert (supports_draft_journal());
			for ( ; it != end; ++it)
			{
				wxString wxs = wxString("Record ");
				wxs += std8_to_wx(frequency_description(*it, "every"));
				wxs += ", starting";
				Append(wxs);
				m_frequencies.push_back(*it);
			}
		}
	}
	SetSelection(0);
}

optional<Frequency>
FrequencyCtrl::frequency() const
{
	optional<Frequency> ret;
	vector<Frequency>::size_type index = GetSelection();
	if (supports_ordinary_journal())
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
	ret = m_frequencies[index];
	return ret;
}

void
FrequencyCtrl::set_frequency(optional<Frequency> const& p_maybe_frequency)
{
	if (p_maybe_frequency)
	{
		if (!supports_draft_journal() && !supports_budget_item())
		{
			throw InvalidFrequencyException
			(	"FrequencyCtrl does not support recurring transaction "
				"Frequencies."
			);
		}
		Frequency const freq = value(p_maybe_frequency);
		if (!supports_draft_journal())
		{
			if (freq.step_type() == interval_type::month_ends)
			{
				throw InvalidFrequencyException
				(	"FrequencyCtrl does not support interval_type::month_ends"
					", as it is not calibrated to support DraftJournal "
					"Repeater Frequencies."
				);
			}
		}
		assert (supports_draft_journal() || supports_budget_item());
		vector<Frequency>::const_iterator it = m_frequencies.begin();
		vector<Frequency>::const_iterator const end = m_frequencies.end();
		vector<Frequency>::size_type i = (supports_ordinary_journal()? 1: 0);
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
	if (!supports_ordinary_journal())
	{
		throw InvalidFrequencyException
		(	"FrequencyCtrl does not support \"once-off\" selection."
		);
	}
	assert (supports_ordinary_journal());
	SetSelection(0);
	return;
}

bool
FrequencyCtrl::supports_ordinary_journal() const
{
	return m_supports_ordinary_journal;
}

bool
FrequencyCtrl::supports_draft_journal() const
{
	return m_supports_draft_journal;
}

bool
FrequencyCtrl::supports_budget_item() const
{
	return !(m_supports_ordinary_journal || m_supports_draft_journal);
}

}  // namespace gui
}  // namespace phatbooks
