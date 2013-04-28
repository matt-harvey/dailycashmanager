#include "account_data_view_model.hpp"
#include "account.hpp"
#include "../decimal_variant_data.hpp"
#include "phatbooks_exceptions.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <wx/dataview.h>
#include <wx/variant.h>
#include <cassert>
#include <vector>

using boost::shared_ptr;
using jewel::Decimal;
using std::vector;

// For debugging
#include <jewel/debug_log.hpp>
#include <iostream>
using std::endl;


namespace phatbooks
{
namespace gui
{


AccountDataViewModel::AccountDataViewModel
(	vector<AugmentedAccount> const& p_accounts
)
{
	for
	(	vector<AugmentedAccount>::const_iterator it = p_accounts.begin(),
			end = p_accounts.end();
		it != end;
		++it
	)
	{
		JEWEL_DEBUG_LOG << "Adding AugmentedAccount* to AccountDataViewModel "
		                << "with Account name "
						<< it->account.name()
						<< endl;
		AugmentedAccount* augmented_account = new AugmentedAccount(*it);
		m_accounts.push_back(augmented_account);
	}
}

AccountDataViewModel::~AccountDataViewModel()
{
	for
	(	vector<AugmentedAccount*>::iterator it = m_accounts.begin(),
			end = m_accounts.end();
		it != end;
		++it
	)
	{
		delete *it;
		*it = 0;
	}
}

bool
AccountDataViewModel::IsContainer(wxDataViewItem const& item) const
{
	(void)item;  // Silence compiler warning re. unused parameter.
	return false;
}

wxDataViewItem
AccountDataViewModel::GetParent(wxDataViewItem const& item) const
{
	// Always returns an invalid wxDataViewItem since none
	// of the items have parents that are not the root
	(void)item;  // Silence compiler warning re. unused parameter.
	return wxDataViewItem(0);
}

unsigned int
AccountDataViewModel::GetChildren
(	wxDataViewItem const& item,
	wxDataViewItemArray& children
) const
{
	if (item == wxDataViewItem(0))
	{
		assert (!item.IsOk());
		JEWEL_DEBUG_LOG << "Calling AccountDataViewModel::GetChildren"
		                << endl;
		// Then the item is the root and all the AugmentedAccounts are its
		// children. Deliberately start counting from 1, as 0 is for an
		// invalid wxDataViewItem.
		unsigned int const ret = get_all_items(children);
		JEWEL_DEBUG_LOG << "Number of children gotten: " << ret << endl;
		return ret;
	}
	return 0;
}
	
unsigned int
AccountDataViewModel::GetColumnCount() const
{
	return s_num_columns;
}

wxString
AccountDataViewModel::GetColumnType(unsigned int col) const
{
	switch (col)
	{
	case s_name_column:  // fall through
	case s_account_type_column:
		return wxString("string");
	case s_opening_balance_column:
		return DecimalVariantData::GetTypeStatic();
	default:
		throw AccountDataViewModelException("Invalid column number.");
	}
}

void
AccountDataViewModel::GetValue
(	wxVariant& variant,
	wxDataViewItem const& item,
	unsigned int col
) const
{
	if (!item.IsOk())
	{
		throw AccountDataViewModelException("Invalid wxDataViewItem.");
	}
	AugmentedAccount* augmented_account =
		static_cast<AugmentedAccount*>(item.GetID());
	if (!augmented_account)
	{
		throw AccountDataViewModelException
		(	"Invalid pointer to AugmentedAccount."
		);
	}
	wxVariantData* data = 0;
	switch (col)
	{
	case s_name_column:
		variant = bstring_to_wx(augmented_account->account.name());
		return;
	case s_account_type_column:
		variant = bstring_to_wx
		(	account_type_to_string(augmented_account->account.account_type())
		);
		return;
	case s_opening_balance_column:
		data = new DecimalVariantData
		(	augmented_account->technical_opening_balance
		);
		variant.SetData(data);
		return;
	default:
		throw AccountDataViewModelException("Invalid column number.");
	}
}

bool
AccountDataViewModel::SetValue
(	wxVariant const& variant,	
	wxDataViewItem const& item,
	unsigned int col
)
{
	if (!item.IsOk())
	{
		throw AccountDataViewModelException("Invalid wxDataViewItem.");
	}
	AugmentedAccount* augmented_account =
		static_cast<AugmentedAccount*>(item.GetID());
	if (!augmented_account)
	{
		throw AccountDataViewModelException
		(	"Invalid pointer to AugmentedAccount."
		);
	}
	DecimalVariantData* dvd = 0;
	switch (col)
	{
	case s_name_column:
		augmented_account->account.set_name
		(	wx_to_bstring(variant.GetString())
		);
		return true;
	case s_account_type_column:
		augmented_account->account.set_account_type
		(	string_to_account_type(wx_to_bstring(variant.GetString()))
		);
		return true;
	case s_opening_balance_column:
		dvd = dynamic_cast<DecimalVariantData*>(variant.GetData());
		if (!dvd)
		{
			throw AccountDataViewModelException("Wrong wxVariant type.");
		}
		augmented_account->technical_opening_balance = dvd->decimal();
		return true;
	default:
		throw AccountDataViewModelException("Invalid column number.");
	}
	return false;
}

unsigned int
AccountDataViewModel::get_all_items(wxDataViewItemArray& items) const
{
	for
	(	vector<AugmentedAccount*>::const_iterator it = m_accounts.begin(),
			end = m_accounts.end();
		it != end;
		++it
	)
	{
		void* item = static_cast<void*>(*it);
		items.Add(wxDataViewItem(item));
	}
	return m_accounts.size();
}





}  // namespace gui
}  // namespace phatbooks
