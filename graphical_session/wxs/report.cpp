#include "report.hpp"
#include "account_type.hpp"
#include "balance_sheet_report.hpp"
#include "pl_report.hpp"
#include "report_panel.hpp"
#include "phatbooks_database_connection.hpp"

using boost::optional;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

Report*
Report::create
(	ReportPanel* p_parent,
	wxSize const& p_size,
	account_super_type::AccountSuperType p_account_super_type,
	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
)
{
	Report* temp = 0;
	switch (p_account_super_type)
	{
	case account_super_type::balance_sheet:
		temp = new BalanceSheetReport
		(	p_parent,
			p_size,
			p_database_connection,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	case account_super_type::pl:
		temp = new PLReport
		(	p_parent,
			p_size,
			p_database_connection,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	default:
		assert (false);
	}
	return temp;
}

Report::Report
(	ReportPanel* p_parent,
	wxSize const& p_size,
	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
):
	wxScrolledWindow
	(	p_parent,
		wxID_ANY,
		wxDefaultPosition,
		p_size,
		wxVSCROLL
	),
	m_database_connection(p_database_connection),
	m_maybe_min_date(p_maybe_min_date),
	m_maybe_max_date(p_maybe_max_date)
{
}

Report::~Report()
{
}

PhatbooksDatabaseConnection&
Report::database_connection()
{
	return m_database_connection;
}

}  // namespace gui
}  // namespace phatbooks
