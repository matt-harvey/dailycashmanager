#include "persistent_journal_dialogue.hpp"
#include "dialogue.hpp"
#include "persistent_journal.hpp"
#include "phatbooks_text_session.hpp"
#include "consolixx/text_session.hpp"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;
typedef TextSession::Menu Menu;
typedef TextSession::MenuItem MenuItem;


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
PersistentJournalDialogue::conduct_editing()
{
	// TODO Implement this
	return;
}



void
PersistentJournalDialogue::present_master_menu()
{
	typedef shared_ptr<MenuItem const> ItemPtr;
	typedef PersistentJournalDialogue::PJD;
	for (bool exiting = false; !exiting; )
	{
		cout << endl << journal << endl;
		Menu menu("Select an action from the above menu: ");
		
		ItemPtr add_entry_item
		(	new MenuItem
			(	"Add a line",
				bind(bind(&PJD::elicit_entry_insertion, this, _1), journal)
			)
		);
		menu.add_item(add_entry_item);
	 
	// TODO Finish this

	return;
}
	

void
PersistentJournalDialogue::add_master_menu_item
(	shared_ptr<TextSession::MenuItem const> const& item
)
{
	// TODO Implement this
	return;
}




}  // namespace phatbooks
