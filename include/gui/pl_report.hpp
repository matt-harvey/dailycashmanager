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

#ifndef GUARD_pl_report_hpp_03798236466850264
#define GUARD_pl_report_hpp_03798236466850264

#include "report.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <jewel/decimal.hpp>
#include <wx/gdicmn.h>
#include <unordered_map>

namespace dcm
{

// begin forward declarations

class DcmDatabaseConnection;

namespace gui
{

class ReportPanel;

// end forward declarations

class PLReport: public Report
{
public:
    PLReport
    (   ReportPanel* p_parent,
        wxSize const& p_size,
        DcmDatabaseConnection& p_database_connection,
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
    (   int p_column,
        jewel::Decimal const& p_total = jewel::Decimal(0, 0),
        int p_count = 0
    );

    void refresh_map();

    void display_body();

    typedef std::unordered_map<sqloxx::Id, jewel::Decimal> Map;
    Map m_map;

};  // class PLReport

}  // namespace gui
}  // namespace dcm

#endif  // GUARD_pl_report_hpp_03798236466850264
