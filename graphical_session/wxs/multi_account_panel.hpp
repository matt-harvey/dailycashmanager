#ifndef GUARD_multi_account_panel_hpp
#define GUARD_multi_account_panel_hpp

#include "account.hpp"
#include "account_type.hpp"
#include "account_type_ctrl.hpp"
#include "commodity.hpp"
#include "decimal_text_ctrl.hpp"
#include <boost/noncopyable.hpp>
#include <jewel/decimal.hpp>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <vector>

namespace phatbooks
{

// begin forward declarations

class Commodity;
class PhatbooksDatabaseConnection;

namespace gui
{

// end forward declarations

/**
 * An instance of this class is to be included in the
 * SetupWizard::BalanceSheetAccountPage. This class represents a scrolled
 * panel in which the user can create and configure multiple Accounts,
 * with the types, descriptions and opening balances.
 *
 * @todo Some functions here duplicate some in Report (e.g. make_text). Find
 * an elegant way to re-use this code.
 *
 * @todo We need to ensure that a the database connection has a default
 * Commodity, or else, we need to pass a Commodity separately to the
 * MultiAccountPanel constructor.
 */
class MultiAccountPanel: public wxScrolledWindow, private boost::noncopyable
{
public:
	MultiAccountPanel
	(	wxWindow* p_parent,
		wxSize const& p_size,
		PhatbooksDatabaseConnection& m_database_connection,
		account_super_type::AccountSuperType p_account_super_type,
		Commodity const& p_commodity
	);
		
	virtual ~MultiAccountPanel();

	/**
	 * @returns the width required to fit all the widgets in a
	 * MultiAccountPanel, along with a standard border and scrollbar.
	 */
	static int required_width();

	void set_commodity(Commodity const& p_commodity);

	struct AugmentedAccount
	{
		AugmentedAccount
		(	PhatbooksDatabaseConnection& p_database_connection,
			Commodity const& p_commodity
		);
		AugmentedAccount
		(	Account const& p_account,
			jewel::Decimal const& p_technical_opening_balance
		);
		Account account;
		jewel::Decimal technical_opening_balance;
	};

	/**
	 * Populates \e out with AugmentedAccounts corresponding to what has
	 * been entered in the MultiAccountPanel by the user. The
	 * Accounts in the AugmentedAccounts will not have been saved (so
	 * will not have IDs). Also, no opening balance Journals will be saved
	 * (it is the client's responsibility to create opening balance Journals,
	 * if they desire to do so, using the information provided in the
	 * AugmentedAccounts. The Accounts in the AugmentedAccounts will have
	 * their commodity() attribute initialized to
	 * database_connection().default_commodity().
	 *
	 * \e out should be empty when passed to this function. It is the caller's
	 * responsibility to ensure this.
	 */
	void selected_augmented_accounts
	(	std::vector<AugmentedAccount>& out
	);

	/**
	 * @return \e true if and only if the Account names entered in the
	 * MultiAccountPanel are all unique (i.e. different from each other)
	 * on a case-insensitive basis, and are all non-blank. If the return
	 * value is \e false, then an user-friendly error message will be
	 * placed inside \e error_message.
	 */
	bool account_names_valid(wxString& p_error_message) const;
	
protected:
	wxGridBagSizer& top_sizer();
	int current_row() const;
	void increment_row();
	void make_text
	(	wxString const& p_text,
		int p_column,
		int p_alignment_flags = wxALIGN_LEFT
	);
	PhatbooksDatabaseConnection& database_connection();
	PhatbooksDatabaseConnection const& database_connection() const;

private:

	void configure_scrollbars();

	account_super_type::AccountSuperType m_account_super_type;
	int m_current_row;
	wxGridBagSizer* m_top_sizer;
	PhatbooksDatabaseConnection& m_database_connection;
	Commodity m_commodity;

	std::vector<wxTextCtrl*> m_account_name_boxes;
	std::vector<AccountTypeCtrl*> m_account_type_boxes;
	std::vector<wxTextCtrl*> m_description_boxes;
	std::vector<DecimalTextCtrl*> m_opening_balance_boxes;

};  // class MultiAccountPanel

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_multi_account_panel_hpp
