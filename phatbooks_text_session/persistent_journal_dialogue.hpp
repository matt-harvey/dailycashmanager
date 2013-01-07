#ifndef GUARD_persistent_journal_dialogue_hpp
#define GUARD_persistent_journal_dialogue_hpp


#include "dialogue.hpp"

namespace phatbooks
{


class PhatbooksTextSession;
class PersistentJournal;


class PersistentJournalDialogue
{
public:
	PersistentJournalDialogue
	(	PhatbooksTextSession const& p_session,
		PersistentJournal& p_journal
	);
	
	


private:
	void do_run();
	PersistentJournal& m_journal;



};  // PersistentJournalDialogue


}  // namespace phatbooks


#endif  // GUARD_persistent_journal_dialogue_hpp
