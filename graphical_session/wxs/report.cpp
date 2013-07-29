#include "report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "balance_sheet_report.hpp"
#include "entry.hpp"
#include "ordinary_journal.hpp"
#include "pl_report.hpp"
#include "report_panel.hpp"
#include "phatbooks_database_connection.hpp"
#include "sizing.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/optional.hpp>

using boost::optional;
using jewel::value;

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
	m_top_sizer(0),
	m_database_connection(p_database_connection),
	m_min_date(database_connection().opening_balance_journal_date()),
	m_maybe_max_date(p_maybe_max_date)
{
	if (p_maybe_min_date)
	{
		gregorian::date const provided_min_date = value(p_maybe_min_date);
		if (provided_min_date > m_min_date)
		{
			m_min_date = provided_min_date;
		}
	}
	m_top_sizer = new wxGridBagSizer(standard_gap(), standard_gap());
	SetSizer(m_top_sizer);
}

Report::~Report()
{
}

gregorian::date
Report::min_date() const
{
	return m_min_date;
}

optional<gregorian::date>
Report::maybe_max_date() const
{
	return m_maybe_max_date;
}

wxGridBagSizer&
Report::top_sizer()
{
	assert (m_top_sizer);
	return *m_top_sizer;
}

void
Report::update_for_new(OrdinaryJournal const& p_journal)
{
	// TODO HIGH PRIORITY Implement
}

void
Report::update_for_amended(OrdinaryJournal const& p_journal)
{
	// TODO HIGH PRIORITY Implement
}

void
Report::update_for_new(Account const& p_account)
{
	// TODO HIGH PRIORITY Implement
}

void
Report::update_for_amended(Account const& p_account)
{
	// TODO HIGH PRIORITY Implement
}

void
Report::update_for_deleted(std::vector<Entry::Id> const& p_doomed_ids)
{
	// TODO HIGH PRIORITY Implement
}

void
Report::generate()
{
	// TODO Can we factor up more shared code here?
	do_generate();
	configure_scrollbars();
	m_top_sizer->Fit(this);
	m_top_sizer->SetSizeHints(this);
	FitInside();
	GetParent()->Fit();
	// Layout();
	return;
}

void
Report::configure_scrollbars()
{
	SetScrollRate(0, 10);
	FitInside();
	return;
}

PhatbooksDatabaseConnection&
Report::database_connection()
{
	return m_database_connection;
}

}  // namespace gui
}  // namespace phatbooks
