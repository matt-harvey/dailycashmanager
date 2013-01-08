#ifndef GUARD_phatbooks_text_session_hpp
#define GUARD_phatbooks_text_session_hpp

#include "consolixx/text_session.hpp"
#include "session.hpp"
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


/** \file phatbooks_text_session.hpp
 *
 * \brief Header for text/console user interface for Phatbooks.
 *
 * \author Matthew Harvey
 * \date 04 July 2012.
 *
 * Copyright (c) 2012, Matthew Harvey. All rights reserved.
 */



namespace phatbooks
{

// FORWARD DECLARATIONS

class Account;
class DraftJournal;
class Entry;
class Journal;
class PersistentJournal;
class ProtoJournal;
class PhatbooksDatabaseConnection;
class Repeater;

// CLASSES

/**
 * @todo HIGH PRIORITY When the user provides a filename to open,
 * the application needs to detect if that filename exists
 * but is not a Phatbooks database file. If it is not a Phatbooks
 * database file, it needs to be impossible for the user to proceed
 * with creating a database connection to that file.
 */
class PhatbooksTextSession:
	public Session,  // Phatbooks Session
	private consolixx::TextSession
{
public:

	PhatbooksTextSession();
	virtual ~PhatbooksTextSession();	


private:
		

	/**
	 * Implements virtual function do_run, inherited from
	 * phatbooks::Session.
	 *
	 * Runs the user session, i.e. executes the chain of events such as
	 * displaying menus, reacting to user input, etc. that constitutes
	 * a Phatbooks text session.
	 * If the file does not already exist, the user will be asked whether
	 * they want to create a file with this name.
	 *
	 * As this function calls many other functions,
	 * which will vary in turn depending on the user's input, it is probably
	 * impracticable to attempt to document every exception that might be
	 * thrown.
	 *
	 * @returns \c 1 if there is some kind of error condition that is not
	 * manifested as an exception; or \c 0 on successful
	 * completion.
	 *
	 * @param filename Is the name of a file to which a database connection
	 * should be opened for the session.
	 */
	int do_run(std::string const& filename);


	// This effects only the presentation to the user when
	// constructing the journal and is not stored as an aspect of
	// the eventual journal object.
	enum TransactionType
	{
		expenditure_transaction = 0,
		revenue_transaction,
		balance_sheet_transaction,
		envelope_transaction,
		generic_transaction
	};
	static int const num_transaction_types = 5;

	// Various phrases used in interacting with the user. The wording
	// used for each phrase may vary with TransactionType
	enum PhraseType
	{
		account_prompt = 0,
		amount_prompt,
		secondary_account_prompt,
		secondary_account_prompt_simple,
		secondary_account_prompt_plural
	};
	static int const num_phrase_types = 5;

	// As the user adds Entries to an Account, they are in a particular
	// "phase". The phase determines various aspects of the user-facing
	// dialogue.
	enum TransactionPhase
	{
		primary_phase,
		secondary_phase
	};
	static int const num_transaction_phases = 2;

	// Retrieve the user-facing string appropriate for a given
	// TransactionType and PhraseType
	std::string dialogue_phrase
	(	TransactionType transaction_type,
		PhraseType dialogue_phrase
	);

	// Returns whether a given Account is a valid selection by the
	// user for a given TransactionType and TransactionPhase.
	// Places a user-friendly description of what \e would be valid
	// in validity_description (e.g. "asset or liability account").
	bool account_is_valid
	(	TransactionType transaction_type,
		TransactionPhase transaction_phase,
		Account const& account,
		std::string& validity_description
	);

	// Elicit an Account of a type that is valid for
	// the given transaction_type and transaction_phase.
	// If allow_empty_to_throw, the function will return
	// an uninitialized optional if the user enters an empty
	// string. Otherwise, it will re-prompt the user until
	// a valid account is entered, and an initialized optional
	// will always be returned.
	boost::optional<Account> elicit_valid_account
	(	TransactionType transaction_type,
		TransactionPhase transaction_phase,
		bool allow_empty_to_throw = false
	);
		
	TransactionType elicit_transaction_type();

	void elicit_primary_entries
	(	ProtoJournal& journal,
		TransactionType transaction_type
	);

	void elicit_secondary_entries
	(	ProtoJournal& journal,
		TransactionType transaction_type
	);

	void finalize_journal(ProtoJournal& journal);
	void finalize_ordinary_journal(OrdinaryJournal& journal);
	void finalize_draft_journal(DraftJournal& journal, bool autopost);




	// boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;
	// static std::string const s_application_name;

	/**
	 * Enter a dialogue with the user for creating a new Commodity, which on
	 * creation will be persisted to the database.
	 *
	 * This function does not throw, except for possibly \c std::bad_alloc
	 * in extreme conditions.
	 *
	 * @todo This is not very user-friendly. The user is asked about
	 * precision, base commodities and so on. These concepts are not well
	 * explained, and furhermore, the user shouldn't have to think about
	 * have to think about commodities at all unless they want to deal
	 * with foreign currencies or investments. Work out a better way to set
	 * up commodities in the database. Also there is no way for the user to
	 * abort the dialogue if they so choose, and there is no way other than
	 * asking the user to "try again" for them to "negotiate" things if the
	 * precision or conversion rate they entered exceeds limits. Also there
	 * is no exception handling at the point the commodity is actually created
	 * and inserted into the database.
	 *
	 * @todo LOW PRIORITY There is code repetition in the part where the
	 * maximum precision is presented to the user and so on. Also, the maximum
	 * precision can probably be a bit more than 6 places. (Maybe it should
	 * actually be a function of the maximum precision of the Decimal type.)
	 */
	void elicit_commodity();

	/**
	 * Enter a dialogue with the user for creating a new Account, which on
	 * creation will be persisted to the database.
	 *
	 * This function does not throw, except possibly \c std::bad_alloc in
	 * extreme conditions.
	 *
	 * @todo Verify throwing behaviour.
	 *
	 * @todo Most users think of "account" as meaning "bank account". The
	 * wording in the dialogue that the user sees here should possibly
	 * change to reflect this.
	 *
	 * @todo We need to stop the user from selecting a native currency
	 * for P&L accounts that is not the native currency of the entity
	 * itself.
	 *
	 * @todo HIGH PRIORITY Need to ensure there is at least one comodity in
	 * the database before the user enters this dialogue. The best
	 * solution for this would be to force the user to create the native
	 * commodity for the entity as a whole, immediately they create a new
	 * entity (i.e. new database).
	 *
	 * @todo In the part of the implementation where the user is asked for
	 * a commodity abbreviation, this should really be done by providing a
	 * list of the available commodities from which to select. This could
	 * possibly be done using a \c Menu object.
	 *
	 * @todo There is some code repetition here, between \c elicit_commodity
	 * and \c elicit_account. This would best be eliminated by creating
	 * an intermediate abstraction level, with functions to ask the user
	 * for e.g. an existing account name string, or an non-existing commodity
	 * string, etc..
	 *
	 * @todo Consider making account names case-insensitive.
	 *
	 * @todo We need to stop the user from assigning a currency other
	 * than the entity's native currency to profit and loss accounts.
	 */
	void elicit_account();

	/**
	 * Elicit from user the name of an existing account.
	 *
	 * This function does not throw, except possibly \c std::bad_alloc in
	 * extreme conditions.
	 *
	 * @param accept_empty should be passed true if we want the user to
	 * be able to "escape" from having to select an account by hitting
	 * enter without entering an account name.
	 *
	 * @todo Verify throwing behaviour.
	 */
	std::string elicit_existing_account_name(bool accept_empty = false);

	void elicit_entry_insertion(PersistentJournal& journal);
	void elicit_entry_deletion(PersistentJournal& journal);
	void elicit_entry_amendment(PersistentJournal& journal);
	void elicit_journal_deletion(PersistentJournal& journal);
	void elicit_comment_amendment(PersistentJournal& journal);
	void elicit_repeater_insertion(DraftJournal& journal);
	void elicit_repeater_deletion(DraftJournal& journal);
	void exit_journal_edit_without_saving(PersistentJournal& journal);
	void exit_journal_edit_saving_changes(PersistentJournal& journal);
	void elicit_date_amendment(OrdinaryJournal& journal);


	/**
	 * Enter a dialogue with the user for creating a new Journal.
	 *
	 * This function does not throw, except possibly \c std::bad_alloc in
	 * extreme conditions.
	 *
	 * @todo Verify throwing behaviour
	 * 
	 * @todo We need to ask the user for an amount in the native currency of
	 * the appropriate account.
	 *
	 * @todo Users should be presented with a list of accounts to choose
	 * from.
	 *
	 * @todo Users should be prevented from entering certain account types
	 * for certain transaction types. For example, and expenditure transaction
	 * should have only expense accounts allowed when an "expense category" is
	 * called for.
	 *
	 * @todo This function is enormous. It needs to be tidied and broken
	 * into sensible chunks.
	 *
	 * @todo Enable multiple Repeater instances to be associated with a
	 * single DraftJournal.
	 */
	void elicit_journal();


	Repeater elicit_repeater();

	/**
	 * WARNING This should be removed from any release version.
	 *
	 * Prompts user for a directory name then imports csv files from that
	 * directory.
	 *
	 * This is a quick hack to enable the developer of Phatbooks to migrate
	 * data from a previous application used only by him.
	 */
	void import_from_nap();

	void notify_autoposts
	(	boost::shared_ptr<std::list<OrdinaryJournal> > journal
	) const;

	void display_balance_sheet();
	void display_draft_journals();
	void display_journal_from_id();
	void display_ordinary_actual_entries();
	void display_envelopes();

	void populate_journal_editing_menu_core
	(	Menu& menu,
		PersistentJournal& journal
	);

	// TODO Refactor in terms of PersistentJournalDialogue
	void conduct_dj_editing(DraftJournal& journal);
	void conduct_oj_editing(OrdinaryJournal& journal);

	void display_posted_journals();
	void display_impending_autoposts();
	void display_commodities_menu();
	void display_accounts_menu();
	void display_utilities_menu();
	
	void wrap_up();

	boost::shared_ptr<Menu> m_main_menu;
};


}  // namespace phatbooks


#endif  // GUARD_phatbooks_text_session_hpp

