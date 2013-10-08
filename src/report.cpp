// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#include "gui/report.hpp"
#include "account.hpp"
#include "account_type.hpp"
#include "finformat.hpp"
#include "ordinary_journal.hpp"
#include "phatbooks_database_connection.hpp"
#include "gui/balance_sheet_report.hpp"
#include "gui/gridded_scrolled_panel.hpp"
#include "gui/locale.hpp"
#include "gui/pl_report.hpp"
#include "gui/report_panel.hpp"
#include "gui/sizing.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/decimal.hpp>
#include <jewel/optional.hpp>
#include <sqloxx/handle.hpp>
#include <wx/gdicmn.h>

using boost::optional;
using jewel::Decimal;
using jewel::value;
using sqloxx::Handle;

namespace gregorian = boost::gregorian;

namespace phatbooks
{
namespace gui
{

Report*
Report::create
(	ReportPanel* p_parent,
	wxSize const& p_size,
	AccountSuperType p_account_super_type,
	PhatbooksDatabaseConnection& p_database_connection,
	optional<gregorian::date> const& p_maybe_min_date,
	optional<gregorian::date> const& p_maybe_max_date
)
{
	Report* temp = 0;
	switch (p_account_super_type)
	{
	case AccountSuperType::balance_sheet:
		temp = new BalanceSheetReport
		(	p_parent,
			p_size,
			p_database_connection,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	case AccountSuperType::pl:
		temp = new PLReport
		(	p_parent,
			p_size,
			p_database_connection,
			p_maybe_min_date,
			p_maybe_max_date
		);
		break;
	default:
		JEWEL_HARD_ASSERT (false);
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
	GriddedScrolledPanel(p_parent, p_size, p_database_connection),
	m_min_date
	(	database_connection().opening_balance_journal_date() +
		gregorian::date_duration(1)
	),
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

void
Report::update_for_new(Handle<OrdinaryJournal> const& p_journal)
{
	(void)p_journal;  // silence compiler re. unused parameter.
	return;
}

void
Report::update_for_amended(Handle<OrdinaryJournal> const& p_journal)
{
	(void)p_journal;  // silence compiler re. unused parameter.
	return;
}

void
Report::update_for_new(Handle<Account> const& p_account)
{
	(void)p_account;  // silence compiler re. unused parameter.
	return;
}

void
Report::update_for_amended(Handle<Account> const& p_account)
{
	(void)p_account;  // silence compiler re. unused parameter.
	return;
}

void
Report::update_for_deleted(std::vector<sqloxx::Id> const& p_doomed_ids)
{
	(void)p_doomed_ids;
	return;
}

void
Report::generate()
{
	do_generate();
	// GetParent()->Layout();
	// m_top_sizer->Fit(this);
	// m_top_sizer->SetSizeHints(this);
	FitInside();
	// Layout();
	return;
}

}  // namespace gui
}  // namespace phatbooks
