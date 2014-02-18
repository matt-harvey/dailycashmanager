/*
 * Copyright 2014 Matthew Harvey
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

#ifndef GUARD_repeater_firing_result_hpp_8121989447680297
#define GUARD_repeater_firing_result_hpp_8121989447680297

#include <boost/date_time/gregorian/gregorian.hpp>
#include <sqloxx/id.hpp>

namespace dcm
{

/**
 * Used for conveying information about the result of an attempt to update
 * a single Repeater one time.
 */
class RepeaterFiringResult
{
public:

	/**
	 * @param p_draft_journal_id the Id of the DraftJournal with which the
	 * Repeater is associated.
	 *
	 * @param p_firing_date the date on which the Repeater was fired (or, if
	 * unsuccessful, the date on which it was supposed to fire).
	 *
	 * @param p_successful pass this \e true if an only if the firing attempt
	 * was successful.
	 */
	RepeaterFiringResult
	(	sqloxx::Id p_draft_journal_id,
		boost::gregorian::date const& p_firing_date,
		bool p_successful
	);

	RepeaterFiringResult() = delete;
	RepeaterFiringResult(RepeaterFiringResult const& rhs) = default;
	RepeaterFiringResult(RepeaterFiringResult&& rhs) = default;
	RepeaterFiringResult& operator=(RepeaterFiringResult const& rhs) = default;
	RepeaterFiringResult& operator=(RepeaterFiringResult&& rhs) = default;
	~RepeaterFiringResult() = default;

	/**
	 * @returns the Id of the DraftJournal with which the Repeater is
	 * associated.
	 */
	sqloxx::Id draft_journal_id() const;

	/**
	 * @returns the date on which the Repeater was fired (or, if unsuccessful,
	 * the date on which it was supposed to fire).
	 */
	boost::gregorian::date firing_date() const;

	/**
	 * @returns \e true if and only if the firing attempt was unsuccessful.
	 */
	bool successful() const;

	/**
	 * Mark RepeaterFiringResult as successful.
	 */
	void mark_as_successful();

private:

	sqloxx::Id m_draft_journal_id;
	boost::gregorian::date m_firing_date;
	bool m_successful;

};  // class RepeaterFiringResult

}  // namespace dcm

#endif  // GUARD_repeater_firing_result_hpp_8121989447680297
