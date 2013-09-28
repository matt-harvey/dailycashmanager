// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_frequency_ctrl_hpp_5481597355325519
#define GUARD_frequency_ctrl_hpp_5481597355325519

#include "frequency.hpp"
#include <boost/optional.hpp>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/windowid.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

// End forward declarations

/**
 * Widget for the the user to select a Frequency (for a DraftJournal), or
 * else to select no Frequency (for an OrdinaryJournal).
 */
class FrequencyCtrl: public wxComboBox
{
public:

	/**
	 * @param p_parent parent window
	 *
	 * @param p_id id
	 *
	 * @param p_size size
	 *
	 * @param p_supports_ordinary_journal pass \e true to support
	 * "Frequencies" that are  to an OrdinaryJournal (as opposed to
	 * DraftJournal). The only such Frequency is "once-off" (which is
	 * not actually represented by a Frequency at all).
	 *
	 * @param p_supports_draft_journal pass \e true to support Frequencies
	 * that are required for repeating DraftJournals (i.e. DraftJournals
	 * with at least one Repeater).
	 *
	 * If both p_supports_ordinary_journal and p_supports_draft_journal are
	 * passed e\ false, then the FrequencyCtrl will display text appropriate
	 * to selected a Frequency for the purpose of a BudgetItem (as opposed
	 * to a Journal).
	 */
	FrequencyCtrl
	(	wxWindow* p_parent,
		wxWindowID p_id,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& p_database_connection,
		bool p_supports_ordinary_journal = false,
		bool p_supports_draft_journal = false
	);

	FrequencyCtrl(FrequencyCtrl const&) = delete;
	FrequencyCtrl(FrequencyCtrl&&) = delete;
	FrequencyCtrl& operator=(FrequencyCtrl const&) = delete;
	FrequencyCtrl& operator=(FrequencyCtrl&&) = delete;

	virtual ~FrequencyCtrl() = default;

	/**
	 * If no Frequency is selected (i.e. "Once off" or equivalent is selected)
	 * then this returns an uninitialized boost::optional<Frequency>.
	 * If a Frequency is selected, then this returns a
	 * boost::optional<Frequency> that has been initialized with that
	 * Frequency.
	 */
	boost::optional<Frequency> frequency() const;

	/**
	 * If p_maybe_frequency is passed an uninitialized optional, then
	 * the FrequencyCtrl will display "Once off" or equivalent, or else
	 * will throw InvalidFrequencyException if OrdinaryJournals are not
	 * supported.
	 *
	 * If p_maybe_frequency is passed an initialized optional, then
	 * the FrequencyCtrl will display text describing the Frequency with
	 * which it has been initialized, or else will throw
	 * InvalidFrequencyException if DraftJournals are not supported.
	 */
	void set_frequency(boost::optional<Frequency> const& p_maybe_frequency);

private:
	bool supports_ordinary_journal() const;
	bool supports_draft_journal() const;
	bool supports_budget_item() const;

	std::vector<Frequency> m_frequencies;

	PhatbooksDatabaseConnection& m_database_connection;
	bool const m_supports_ordinary_journal;
	bool const m_supports_draft_journal;


};  // class FrequencyCtrl




}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_frequency_ctrl_hpp_5481597355325519
