// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_setup_wizard_hpp_8623281646810137
#define GUARD_setup_wizard_hpp_8623281646810137

#include "account_type.hpp"
#include "commodity.hpp"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <sqloxx/handle.hpp>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dataview.h>
#include <wx/filedlg.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/treelist.h>
#include <wx/validate.h>
#include <wx/window.h>
#include <wx/wizard.h>
#include <memory>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class AugmentedAccount;
class PhatbooksDatabaseConnection;

namespace gui
{

class Frame;
class MultiAccountPanel;

// End forward declarations


/**
 * Represents the wizard that is presented to GUI users when they first
 * start up the application.
 *
 * @todo Reinstate AccountPage - or rather pages - to offer user the chance
 * to set up Accounts and opening balances (with suggested defaults)
 * prior to first entering the main window. There should be page for
 * balance sheet Accounts, followed by a page for P&L Accounts
 * ("categories").
 *
 * @todo In the FilepathPage, the controls for the directory and the
 * filepath look like you can write in them, but actually you can't. This
 * misleads the user and is bad (which is pointed out in the "GUI Bloopers"
 * book). Fix it.
 *
 * @todo Ensure that Account names in m_pl_account_page do not
 * conflict with those in m_balance_sheet_account_page.
 */
class SetupWizard: public wxWizard
{
public:
	
	/**
	 * Precondition: p_database_connection is not valid (that's the whole
	 * reason we are calling the wizard...).
	 */
	SetupWizard(PhatbooksDatabaseConnection& p_database_connection);

	SetupWizard(SetupWizard const&) = delete;
	SetupWizard(SetupWizard&&) = delete;
	SetupWizard& operator=(SetupWizard const&) = delete;
	SetupWizard& operator=(SetupWizard&&) = delete;
	~SetupWizard();

	void run();

	/**
	 * @returns a standard size to be used for text controls
	 * (wxTextCtrl, wxComboBox) in the pages of this wizard.
	 * The returned wxSize is intended to be used with wxDLG_UNIT macro
	 * when initializing the controls' sizes.
	 */
	static wxSize standard_text_box_size();
	
	sqloxx::Handle<Commodity> selected_currency() const;

	void set_assumed_currency(sqloxx::Handle<Commodity> const& p_commodity);

	jewel::Decimal total_opening_balance() const;

	class AccountPage;

private:

	void selected_augmented_accounts
	(	std::vector<AugmentedAccount>& out
	) const;

	class FilepathValidator;
	class FilepathPage;

	void render_account_pages();

	/**
	 * Set the default Commodity for m_database_connection, based on
	 * the currency selected by the user.
	 *
	 * Should be called only after calling RunWizard().
	 */
	void configure_default_commodity();

	/**
	 * Open m_database_connection to filepath selected by the user
	 * in the wizard, creating the file in the process.
	 *
	 * Should be called only after calling RunWizard(), and after calling
	 * configure_default_commodity().
	 */
	void create_file();

	void delete_file();

	/**
	 * Reflect in the database the Accounts selected by user in
	 * the wizard.
	 *
	 * Should be called only after calling RunWizard(), and after calling
	 * configure_default_commodity() and create_file().
	 */
	void configure_accounts();

	PhatbooksDatabaseConnection& m_database_connection;
	FilepathPage* m_filepath_page;
	AccountPage* m_balance_sheet_account_page;
	AccountPage* m_pl_account_page;
	
};  // SetupWizard


/**
 * To facilitate validation of filepath entered in
 * SetupWizard::FilepathPage.
 *
 * @todo The implementation of this is rather messy. Make it
 * nicer.
 */
class SetupWizard::FilepathValidator: public wxValidator
{
public:
	FilepathValidator(boost::filesystem::path* p_filepath);
	FilepathValidator(FilepathValidator const& rhs);

	FilepathValidator(FilepathValidator&&) = delete;
	FilepathValidator& operator=(FilepathValidator const&) = delete;
	FilepathValidator& operator=(FilepathValidator&&) = delete;
	virtual ~FilepathValidator();

	/*** Functions inherited from wxValidator virtuals ***/

	/**
	 * @param parent should be passed the wxTextCtrl in
	 * a FilepathPage,
	 * that contains the filename entered by the user.
	 * Then validator then validates this by ensuring that:\n
	 * (a) it can be a valid filename; and\n
	 * (b) the filepath consisting of text in \e parent, appended
	 * to the directory also entered into the FilepathPage to which
	 * the \e parent belongs, does not already exist.
	 */
	bool Validate(wxWindow* parent);

	bool TransferFromWindow();

	bool TransferToWindow();

	wxObject* Clone() const; 

private:
	boost::filesystem::path* m_filepath;
};


/**
 * This is the first page the user sees when opening the application
 * without a file selected. This welcomes the user and prompts them to
 * select a file.
 *
 * @todo In currency selection box, it would probably be better to associate
 * data with the items using SetItemData etc., than using crude mapping from
 * strings etc..
 */
class SetupWizard::FilepathPage: public wxWizardPageSimple
{
public:
	friend class FilepathValidator;

	FilepathPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);
	FilepathPage(FilepathPage const&) = delete;
	FilepathPage(FilepathPage&&) = delete;
	FilepathPage& operator=(FilepathPage const&) = delete;
	FilepathPage& operator=(FilepathPage&&) = delete;
	virtual ~FilepathPage();

	boost::optional<boost::filesystem::path> selected_filepath() const;
	sqloxx::Handle<Commodity> selected_currency() const;
private:

	void on_directory_button_click(wxCommandEvent& event);
	void on_wizard_page_changing(wxWizardEvent& event);

	std::vector<sqloxx::Handle<Commodity> > const m_currencies;

	wxBoxSizer* m_top_sizer;
	wxBoxSizer* m_filename_row_sizer;
	wxBoxSizer* m_directory_row_sizer;
	wxTextCtrl* m_directory_ctrl;
	wxButton* m_directory_button;
	wxTextCtrl* m_filename_ctrl;
	wxComboBox* m_currency_box;

	std::unique_ptr<boost::filesystem::path> m_selected_filepath;

	static int const s_directory_button_id = wxID_HIGHEST + 1;

	DECLARE_EVENT_TABLE()

};  // SetupWizard::FilepathPage


/**
 * Represents a page for setting up Accounts in a SetupWizard.
 * Abstract base class - more specific Account page types derive
 * from this.
 */
class SetupWizard::AccountPage: public wxWizardPageSimple
{
public:
	AccountPage
	(	SetupWizard* p_parent,
		AccountSuperType p_account_super_type,
		PhatbooksDatabaseConnection& p_database_connection
	);

	AccountPage(AccountPage const&) = delete;
	AccountPage(AccountPage&&) = delete;
	AccountPage& operator=(AccountPage const&) = delete;
	AccountPage& operator=(AccountPage&&) = delete;

	virtual ~AccountPage();

	void render();

	void selected_augmented_accounts
	(	std::vector<AugmentedAccount>& out
	) const;

	void set_commodity(sqloxx::Handle<Commodity> const& p_commodity);

	jewel::Decimal total_amount() const;

	/**
	 * @returns the total_amount() of the balance sheet AccountPage in the
	 * same SetupWizard as this AccountPage.
	 *
	 * WARNING This is rather convoluted.
	 */
	jewel::Decimal total_balance_sheet_amount() const;

protected:
	PhatbooksDatabaseConnection& database_connection() const;
	SetupWizard const& parent() const;
	wxGridBagSizer& top_sizer();
	int current_row() const;
	void increment_row();

	bool account_names_valid(wxString& p_error_message) const;
	bool account_types_valid(wxString& p_error_message) const;

private:

	void on_pop_row_button_click(wxCommandEvent& event);
	void on_push_row_button_click(wxCommandEvent& event);
	void on_wizard_page_changing(wxWizardEvent& event);
	void on_wizard_page_changed(wxWizardEvent& event);

	void render_main_text();
	void render_buttons();
	void render_account_view();
	void refresh_pop_row_button_state();
	wxString main_text() const;

	static unsigned int const s_pop_row_button_id = wxID_HIGHEST + 1;
	static unsigned int const s_push_row_button_id = s_pop_row_button_id + 1;

	PhatbooksDatabaseConnection& m_database_connection;
	AccountSuperType m_account_super_type;
	size_t const m_min_num_accounts;
	int m_current_row; 
	wxButton* m_pop_row_button;
	wxButton* m_push_row_button;
	wxGridBagSizer* m_top_sizer;
	MultiAccountPanel* m_multi_account_panel;
	SetupWizard const& m_parent;

	DECLARE_EVENT_TABLE()
};


}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_setup_wizard_hpp_8623281646810137
