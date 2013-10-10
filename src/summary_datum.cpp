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
