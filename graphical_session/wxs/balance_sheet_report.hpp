#ifndef GUARD_balance_sheet_report_hpp
#define GUARD_balance_sheet_report_hpp

#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/decimal.hpp>
#include <wx/gdicmn.h>
#include <wx/string.h>

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

	virtual ~BalanceSheetReport();

private:
	virtual void do_generate();

	void refresh_balance_map();

	void display_text();

	void make_text
	(	wxString const& p_text,
		int p_column,
		int p_flags = wxALIGN_LEFT
	);

	void make_number_text(jewel::Decimal const& p_amount, int p_column);

	struct BalanceDatum
	{
		BalanceDatum();
		BalanceDatum(Account const& p_account);
		jewel::Decimal opening_balance;
		jewel::Decimal closing_balance;
	};

	typedef boost::unordered_map<Account::Id, BalanceDatum> BalanceMap;
	BalanceMap m_balance_map;

	int m_next_row;

};  // class BalanceSheetReport

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_balance_sheet_report_hpp
