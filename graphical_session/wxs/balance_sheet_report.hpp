#ifndef GUARD_balance_sheet_report_hpp
#define GUARD_balance_sheet_report_hpp

#include "account.hpp"
#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/decimal.hpp>
#include <wx/gdicmn.h>

namespace phatbooks
{

// begin forward declarations

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

	virtual ~BalanceSheetReport();

private:
	virtual void do_generate();

	void refresh_balance_map();
	void display_text();

	struct BalanceDatum
	{
		BalanceDatum(): opening_balance(0, 0), closing_balance(0, 0)
		{
		}

		BalanceDatum(Account const& p_account):
			opening_balance(0, p_account.commodity().precision()),
			closing_balance(0, p_account.commodity().precision())
		{
		}

		jewel::Decimal opening_balance;
		jewel::Decimal closing_balance;
	};

	typedef boost::unordered_map<Account::Id, BalanceDatum> BalanceMap;
	BalanceMap m_balance_map;

};  // class BalanceSheetReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_balance_sheet_report_hpp
