#ifndef GUARD_account_data_view_model_hpp
#define GUARD_account_data_view_model_hpp

#include "account.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <jewel/decimal.hpp>
#include <wx/dataview.h>
#include <wx/variant.h>
#include <vector>

namespace phatbooks
{
namespace gui
{

/**
 * This class, by subclassing wxDataViewModel, facilites
 * the presentation of phatbooks::Account-related information
 * in the form of a wxDataViewCtrl to be used when editing Accounts.
 *
 * There are three columns:\e
 * 	Account name;\e
 * 	Account type; and\e
 * 	Account opening balance.
 *
 * The implementation of this class is somewhat grotesque, mainly on
 * account of having to satisfy the old-style wxWidgets API.
 *
 * @todo Review cases where all the hideous casts might fail and so
 * on, and prevent silent failure in all such cases.
 *
 * @todo Use anonymous-namespace-scoped constants instead of literal
 * column numbers throughout implementation.
 *
 * @todo The way I have implemented identifiers (using a map) is needlessly
 * complex. The identifiers should basically be glorified pointers to
 * the objects in the container - see wxWidgets' sample code.
 *
 * @todo Who controls the memory for wxDataViewItem?
 */
class AccountDataViewModel: public wxDataViewModel
{
public:

	/**
	 * We need to hold the Account together with its opening
	 * balance.
	 */
	struct AugmentedAccount
	{
		Account account;
		jewel::Decimal technical_opening_balance;
	};

	/**
	 * Construct AccountDataViewModel with a container of the
	 * Accounts to be modelled.
	 */
	AccountDataViewModel(std::vector<AugmentedAccount> const& p_accounts);
	
	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 * Indicates whether \e item is a container, i.e. can have
	 * child items.
	 */
	virtual bool IsContainer(wxDataViewItem const& item) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 *
	 * @returns the wxDataViewItem that represents the parent of item, or
	 * an invalid wxDataViewItem if the root item is the parent
	 * item.
	 */
	virtual wxDataViewItem GetParent(wxDataViewItem const& item) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 */
	virtual unsigned int GetChildren
	(	wxDataViewItem const& item,
		wxDataViewItemArray& children
	) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 *
	 * Indicates the number of columns in the model.
	 */
	virtual unsigned int GetColumnCount() const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 *
	 * Indicates what type of data is stored in the column
	 * specified by \e col.
	 */
	virtual wxString GetColumnType(unsigned int col) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 */
	virtual void GetValue
	(	wxVariant& variant,
		wxDataViewItem const& item,
		unsigned int col
	) const;

	/**
	 * Function inherited as pure virtual from wxDataViewModel.
	 */
	virtual bool SetValue
	(	wxVariant const& variant,
		wxDataViewItem const& item,
		unsigned int col
	);
	
private:

	unsigned int get_all_items(wxDataViewItemArray& items) const;

	static unsigned int const s_num_columns = 3;

	unsigned int m_least_available_identifier;

	// Map identifiers to AugmentedAccounts
	typedef boost::unordered_map
	<	unsigned int,
		boost::shared_ptr<AugmentedAccount>
	> Map;
	Map m_accounts;

};  // AccountDataViewModel


}  // namespace gui
}  // namespace phatbooks


#endif  // GUARD_account_data_view_model_hpp
