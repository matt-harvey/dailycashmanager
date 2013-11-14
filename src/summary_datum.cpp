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

#include "gui/summary_datum.hpp"
#include <jewel/decimal.hpp>
#include <wx/string.h>
#include <utility>

using jewel::Decimal;
using std::move;

namespace phatbooks
{
namespace gui
{

SummaryDatum::SummaryDatum(wxString const& p_label, Decimal const& p_amount):
	m_label(p_label),
	m_amount(p_amount)
{
}

wxString
SummaryDatum::label() const
{
	return m_label;
}

Decimal
SummaryDatum::amount() const
{
	return m_amount;
}

void
SummaryDatum::set_amount(Decimal const& p_amount)
{
	m_amount = p_amount;
	return;
}


}  // namespace gui
}  // namespace phatbooks
