#ifndef GUARD_phatbooks_text_session_hpp
#define GUARD_phatbooks_text_session_hpp

#include "consolixx.hpp"
#include <boost/function.hpp>
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

class PhatbooksDatabaseConnection;

// CLASSES

/**
 * @todo HIGH PRIORITY When the user provides a filename to open,
 * the application needs to detect if that filename exists
 * but is not a Phatbooks database file. If it is not a Phatbooks
 * database file, it needs to be impossible for the user to proceed
 * with creating a database connection to that file.
 */
class PhatbooksTextSession:
	public consolixx::TextSession
{
public:

	PhatbooksTextSession();
	virtual ~PhatbooksTextSession();	

	/**
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
	 * @throw sqloxx::InvalidFilename is \c filename is an empty string.
	 *
	 * @param filename Is the name of a file to which a database connection
	 * should be opened for the session.
	 */
	int run(std::string const& filename);

private:

	boost::shared_ptr<Menu> m_main_menu;
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;


	/**
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

	void display_balance_sheet();
	void display_envelopes();
	void display_profit_and_loss();
	void display_transaction_listing();
	void elicit_actuals_journal();
	void elicit_budget_journal();
	void display_draft_journals();
	void display_posted_journals();
	void display_impending_autoposts();
	void display_commodities_menu();
	void display_accounts_menu();
	void display_utilities_menu();
	
	void wrap_up();

};







}  // namespace phatbooks


#endif  // GUARD_phatbooks_text_session_hpp
