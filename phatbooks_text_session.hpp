#ifndef GUARD_phatbooks_text_session_hpp
#define GUARD_phatbooks_text_session_hpp

#include "consolixx.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>



namespace phatbooks
{

// FORWARD DECLARATIONS

class PhatbooksDatabaseConnection;

// CLASSES

class PhatbooksTextSession:
	public consolixx::TextSession
{
public:
	PhatbooksTextSession();
	void run();
	void run(std::string const& filename);
private:
	boost::shared_ptr<Menu> m_main_menu;
	boost::shared_ptr<PhatbooksDatabaseConnection> m_database_connection;


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
