#include "persistent_journal_dialogue.hpp"
#include "dialogue.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_text_session.hpp"


namespace phatbooks
{

PersistentJournalDialogue::PersistentJournalDialogue
(	PhatbooksTextSession const& p_session,
	PersistentJournal& p_journal
):
	Dialogue(p_session),
	m_journal(p_journal)
{
}



void
PersistentJournalDialogue::do_run()
{
	// TODO Implement this.
	return;
}




}  // namespace phatbooks
