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


#ifndef GUARD_pl_report_hpp_03798236466850264
#define GUARD_pl_report_hpp_03798236466850264

#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <wx/gdicmn.h>
#include <unordered_map>

namespace phatbooks
{

// begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class ReportPanel;

// end forward declarations

class PLReport: public Report
{
public:
	PLReport
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> const& p_maybe_min_date,
		boost::optional<boost::gregorian::date> const& p_maybe_max_date
	);

	PLReport(PLReport const&) = delete;
	PLReport(PLReport&&) = delete;
	PLReport& operator=(PLReport const&) = delete;
	PLReport& operator=(PLReport&&) = delete;
	virtual ~PLReport();

private:
	virtual void do_generate() override;

	/**
	 * @returns an initialized optional only if there is a max_date().
	 */
	boost::optional<int> maybe_num_days_in_period() const;

	/**
	 * Displays "N/A" or the like if p_count is zero. Displays in
	 * current_row().
	 */
	void display_mean
	(	int p_column,
		jewel::Decimal const& p_total = jewel::Decimal(0, 0),
		int p_count = 0
	);

	void refresh_map();

	void display_body();

	typedef std::unordered_map<sqloxx::Id, jewel::Decimal> Map;
	Map m_map;

};  // class PLReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_pl_report_hpp_03798236466850264
