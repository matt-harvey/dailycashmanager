#ifndef GUARD_setup_wizard_hpp
#define GUARD_setup_wizard_hpp

#include "account.hpp"
#include "account_type.hpp"
#include "commodity.hpp"
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/treelist.h>
#include <wx/validate.h>
#include <wx/window.h>
#include <wx/wizard.h>
#include <vector>

namespace phatbooks
{

// Begin forward declarations

class PhatbooksDatabaseConnection;

namespace gui
{

class Frame;

// End forward declarations


/**
 * Represents the wizard that is presented to GUI users when they first
 * start up the application.
 */
class SetupWizard:
	public wxWizard
{
public:
	
	/**
	 * Precondition: p_database_connection is not valid (that's the whole
	 * reason we are calling the wizard...).
	 */
	SetupWizard(PhatbooksDatabaseConnection& p_database_connection);

	void run();

	/**
	 * @returns a standard size to be used for text controls
	 * (wxTextCtrl, wxComboBox) in the pages of this wizard.
	 * The returned wxSize is intended to be used with wxDLG_UNIT macro
	 * when initializing the controls' sizes.
	 */
	static wxSize standard_text_box_size();
	
	Commodity selected_currency() const;


private:

	class FilepathValidator;

	class FilepathPage;
	class AccountPage;

	void render_account_page();

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
	AccountPage* m_account_page;
	
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
class SetupWizard::FilepathPage:
	public wxWizardPageSimple,
	private boost::noncopyable
{
public:
	friend class FilepathValidator;

	FilepathPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);
	~FilepathPage();
	boost::optional<boost::filesystem::path> selected_filepath() const;
	Commodity selected_currency() const;
private:

	void on_directory_button_click(wxCommandEvent& event);
	void on_wizard_page_changing(wxWizardEvent& event);

	std::vector<Commodity> const m_currencies;

	wxBoxSizer* m_top_sizer;
	wxBoxSizer* m_filename_row_sizer;
	wxBoxSizer* m_directory_row_sizer;
	wxTextCtrl* m_directory_ctrl;
	wxButton* m_directory_button;
	wxTextCtrl* m_filename_ctrl;
	wxComboBox* m_currency_box;

	boost::filesystem::path* m_selected_filepath;

	static int const s_directory_button_id = wxID_HIGHEST + 1;

	DECLARE_EVENT_TABLE()

};  // SetupWizard::FilepathPage



class SetupWizard::AccountPage:
	public wxWizardPageSimple,
	private boost::noncopyable
{
public:
	AccountPage
	(	SetupWizard* parent,
		PhatbooksDatabaseConnection& p_database_connection
	);
	~AccountPage();

	std::vector<Account> selected_accounts() const;	

	void render(Commodity const& p_commodity);

	class AccountTreeList;

private:

	PhatbooksDatabaseConnection& m_database_connection;
	wxBoxSizer* m_top_sizer;
	AccountTreeList* m_account_tree;

};  // SetupWizard::AccountPage


/**
 * @todo It would probably be better to associate data with
 * the items in the tree using SetItemData etc., than using
 * crude mapping from strings etc..
 */
class SetupWizard::AccountPage::AccountTreeList:
	public wxTreeListCtrl
{
public:
	AccountTreeList
	(	AccountPage* parent,
		wxSize const& size,
		std::vector<Account> const& p_default_accounts,
		Commodity const& p_commodity
	);
	void selected_accounts(std::vector<Account>& vec) const;

private:
	
	void OnItemChecked(wxTreeListEvent& event);

	static wxString account_type_label
	(	account_type::AccountType p_account_type
	);

	std::vector<Account> const& m_default_accounts;

	DECLARE_EVENT_TABLE()
};


}  // namespace gui
}  // namesapce phatbooks


#endif  // GUARD_setup_wizard_hpp
