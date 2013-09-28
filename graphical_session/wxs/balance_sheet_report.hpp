// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_balance_sheet_report_hpp_8005432485605326
#define GUARD_balance_sheet_report_hpp_8005432485605326

#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
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
	virtual ~BalanceSheetReport() = default;

private:
	virtual void do_generate();

	void refresh_map();

	void display_body();

	struct BalanceDatum
	{
		BalanceDatum() = default;
		explicit BalanceDatum(Account const& p_account);
		BalanceDatum(BalanceDatum const&) = default;
		BalanceDatum(BalanceDatum&&) = default;
		BalanceDatum& operator=(BalanceDatum const&) = default;
		BalanceDatum& operator=(BalanceDatum&&) = default;
		~BalanceDatum() = default;
		jewel::Decimal opening_balance;
		jewel::Decimal closing_balance;
	};

	typedef std::unordered_map<Account::Id, BalanceDatum> BalanceMap;
	BalanceMap m_balance_map;

};  // class BalanceSheetReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_balance_sheet_report_hpp_8005432485605326
