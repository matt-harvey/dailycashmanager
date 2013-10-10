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


#ifndef GUARD_summary_datum_hpp_35246402786693726
#define GUARD_summary_datum_hpp_35246402786693726

#include <jewel/decimal.hpp>
#include <wx/string.h>

namespace phatbooks
{
namespace gui
{

/**
 * Represents a single piece of GUI-displayable financial information,
 * along with a label for that information.
 */
class SummaryDatum
{
public:
	SummaryDatum(wxString const& p_label, jewel::Decimal const& p_amount);
	// synthesised copy constructor, copy assignment and destructor are OK
	// ignore move constructor
	// ignore move assignment
	wxString label() const;
	jewel::Decimal amount() const;
	void set_amount(jewel::Decimal const& p_amount);

private:

	// declare m_label before m_amount to ensure synthesised assignment
	// operator is exception-safe
	wxString m_label;

	jewel::Decimal m_amount;

};  // class SummaryDatum


}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_summary_datum_hpp_35246402786693726
