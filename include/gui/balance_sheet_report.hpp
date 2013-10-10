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


#ifndef GUARD_balance_sheet_report_hpp_8005432485605326
#define GUARD_balance_sheet_report_hpp_8005432485605326

#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <sqloxx/handle_fwd.hpp>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <unordered_map>

namespace phatbooks
{

// begin forward declarations

class Account;
class PhatbooksDatabaseConnection;

namespace gui
{

class ReportPanel;

// end forward declarations

class BalanceSheetReport: public Report
{
public:
	BalanceSheetReport
	(	ReportPanel* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		boost::optional<boost::gregorian::date> p_maybe_min_date,
		boost::optional<boost::gregorian::date> p_maybe_max_date
	);

	BalanceSheetReport(BalanceSheetReport const&) = delete;
	BalanceSheetReport(BalanceSheetReport&&) = delete;
	BalanceSheetReport& operator=(BalanceSheetReport const&) = delete;
	BalanceSheetReport& operator=(BalanceSheetReport&&) = delete;
	virtual ~BalanceSheetReport();

private:
	virtual void do_generate() override;

	void refresh_map();

	void display_body();

	struct BalanceDatum
	{
		BalanceDatum() = default;
		explicit BalanceDatum(sqloxx::Handle<Account> const& p_account);
		BalanceDatum(BalanceDatum const&) = default;
		BalanceDatum(BalanceDatum&&) = default;
		BalanceDatum& operator=(BalanceDatum const&) = default;
		BalanceDatum& operator=(BalanceDatum&&) = default;
		~BalanceDatum() = default;
		jewel::Decimal opening_balance;
		jewel::Decimal closing_balance;
	};

	typedef std::unordered_map<sqloxx::Id, BalanceDatum> BalanceMap;
	BalanceMap m_balance_map;

};  // class BalanceSheetReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_balance_sheet_report_hpp_8005432485605326
