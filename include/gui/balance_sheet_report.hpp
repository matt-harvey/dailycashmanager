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
