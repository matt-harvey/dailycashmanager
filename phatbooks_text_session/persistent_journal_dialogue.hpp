#ifndef GUARD_persistent_journal_dialogue_hpp
#define GUARD_persistent_journal_dialogue_hpp


#include "dialogue.hpp"
#include "consolixx/text_session.hpp"
#include <boost/shared_ptr.hpp>

namespace phatbooks
{


class PhatbooksTextSession;
class PersistentJournal;


class PersistentJournalDialogue:
	public Dialogue
{
public:
	PersistentJournalDialogue
	(	PhatbooksTextSession const& p_session,
		PersistentJournal& p_journal
	);
	void conduct_editing();	

protected:
	void present_master_menu();
	void add_master_menu_item
	(	boost::shared_ptr<TextSession::MenuItem const> const& item
	);

private:
	void create_master_menu();
	PersistentJournal& m_journal;

};  // PersistentJournalDialogue


}  // namespace phatbooks


#endif  // GUARD_persistent_journal_dialogue_hpp
