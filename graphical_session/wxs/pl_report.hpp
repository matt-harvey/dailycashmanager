// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_pl_report_hpp_03798236466850264
#define GUARD_pl_report_hpp_03798236466850264

#include "account_handle.hpp"
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
