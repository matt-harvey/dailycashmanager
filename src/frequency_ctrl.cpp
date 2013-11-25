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

#include "gui/frequency_ctrl.hpp"
#include "frequency.hpp"
#include "interval_type.hpp"
#include "dcm_database_connection.hpp"
#include "dcm_exceptions.hpp"
#include "string_conv.hpp"
#include "gui/combo_box.hpp"
#include <boost/optional.hpp>
#include <jewel/assert.hpp>
#include <jewel/optional.hpp>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

using boost::optional;
using jewel::value;
using std::vector;

namespace dcm
{
namespace gui
{

namespace
{
	vector<Frequency> const& available_frequencies()
	{
		static vector<Frequency> ret;
		if (ret.empty())
		{
			ret.reserve(14);
			ret.push_back(Frequency(1, IntervalType::days));
			ret.push_back(Frequency(1, IntervalType::weeks));
			ret.push_back(Frequency(2, IntervalType::weeks));
			ret.push_back(Frequency(3, IntervalType::weeks));
			ret.push_back(Frequency(4, IntervalType::weeks));
			ret.push_back(Frequency(1, IntervalType::months));
			ret.push_back(Frequency(2, IntervalType::months));
			ret.push_back(Frequency(3, IntervalType::months));
			ret.push_back(Frequency(4, IntervalType::months));
			ret.push_back(Frequency(6, IntervalType::months));
			ret.push_back(Frequency(12, IntervalType::months));
			ret.push_back(Frequency(1, IntervalType::month_ends));
			ret.push_back(Frequency(2, IntervalType::month_ends));
			ret.push_back(Frequency(3, IntervalType::month_ends));
			ret.push_back(Frequency(4, IntervalType::month_ends));
			ret.push_back(Frequency(6, IntervalType::month_ends));
			ret.push_back(Frequency(12, IntervalType::month_ends));
		}
		JEWEL_ASSERT (!ret.empty());
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
	DcmDatabaseConnection& p_database_connection,
	bool p_supports_ordinary_journal,
	bool p_supports_draft_journal
):
	ComboBox
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
	JEWEL_ASSERT (m_frequencies.empty());
	if (supports_ordinary_journal())
	{
		Append(once_off_string());
	}
	if (supports_draft_journal() || supports_budget_item())
	{
		if (supports_budget_item())
		{
			JEWEL_ASSERT (!supports_draft_journal());
			for (Frequency const& freq: available_frequencies())
			{
				if (m_database_connection.supports_budget_frequency(freq))
				{
					Append(std8_to_wx(frequency_description(freq)));
					m_frequencies.push_back(freq);
				}
			}
		}
		else
		{
			JEWEL_ASSERT (supports_draft_journal());
			for (Frequency const& freq: available_frequencies())
			{
				wxString wxs = wxString("Record ");
				wxs += std8_to_wx(frequency_description(freq, "every"));
				wxs += ", starting";
				Append(wxs);
				m_frequencies.push_back(freq);
			}
		}
	}
	SetSelection(0);
}

FrequencyCtrl::~FrequencyCtrl()
{
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
			JEWEL_ASSERT (GetValue() == once_off_string());
			JEWEL_ASSERT (!ret);
			return ret;
		}
		JEWEL_ASSERT (index >= 1);
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
			JEWEL_THROW
			(	InvalidFrequencyException,
				"FrequencyCtrl does not support recurring transaction "
				"Frequencies."
			);
		}
		Frequency const freq = value(p_maybe_frequency);
		if (!supports_draft_journal())
		{
			if (freq.step_type() == IntervalType::month_ends)
			{
				JEWEL_THROW
				(	InvalidFrequencyException,
					"FrequencyCtrl does not support IntervalType::month_ends"
					", as it is not calibrated to support DraftJournal "
					"Repeater Frequencies."
				);
			}
		}
		JEWEL_ASSERT (supports_draft_journal() || supports_budget_item());
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
		JEWEL_HARD_ASSERT (false);
	}
	JEWEL_ASSERT (!p_maybe_frequency);
	if (!supports_ordinary_journal())
	{
		JEWEL_THROW
		(	InvalidFrequencyException,
			"FrequencyCtrl does not support \"once-off\" selection."
		);
	}
	JEWEL_ASSERT (supports_ordinary_journal());
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
}  // namespace dcm
