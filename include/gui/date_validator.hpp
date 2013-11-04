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


#ifndef GUARD_date_validator_hpp_1494637937981001
#define GUARD_date_validator_hpp_1494637937981001

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <wx/validate.h>

namespace phatbooks
{
namespace gui
{

/**
 * Validates a wxString purporting to represent a date.
 */
class DateValidator: public wxValidator
{
public:
	DateValidator
	(	boost::gregorian::date const& p_date,
		bool p_allow_blank,
		boost::optional<boost::gregorian::date> const& p_min_date =
			boost::optional<boost::gregorian::date>()
	);

	DateValidator(DateValidator const& rhs);

	DateValidator(DateValidator&&) = delete;
	DateValidator& operator=(DateValidator const&) = delete;
	DateValidator& operator=(DateValidator&&) = delete;
	~DateValidator() = default;

	/**
	 * @param parent should point to an instance of a wxTextCtrl.
	 */
	bool Validate(wxWindow* parent) override;

	bool TransferFromWindow() override;
	bool TransferToWindow() override;
	wxObject* Clone() const override;

	boost::optional<boost::gregorian::date> date() const;

private:
	bool m_allow_blank;
	boost::optional<boost::gregorian::date> m_date;
	boost::optional<boost::gregorian::date> m_min_date;

};  // class DateValidator

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_date_validator_hpp_1494637937981001
