#include "account_data_view_model.hpp"
#include "account.hpp"
#include "account_type_variant_data.hpp"
#include "decimal_variant_data.hpp"
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

namespace phatbooks
{
namespace gui
{

AccountDataViewModel::AccountDataViewModel
(	vector<AugmentedAccount> const& p_accounts
):
	// Start at 1 as 0 is reserved for invalid wxDataViewItem
	m_least_available_identifier(1)
{
	vector<AugmentedAccount>::size_type i = 0;
	vector<AugmentedAccount>::size_type const max = p_accounts.size();
	while (i != max)
	{
		shared_ptr<AugmentedAccount> account
		(	new AugmentedAccount(p_accounts[i++])
		);
		m_accounts[m_least_available_identifier++] = account;
		assert (i + 1 == m_least_available_identifier);
	}	
}

bool
AccountDataViewModel::IsContainer(wxDataViewItem const& item) const
{
	(void)item;  // silence compiler warning re. unused variable
	return false;
}

wxDataViewItem
AccountDataViewModel::GetParent(wxDataViewItem const& item) const
{
	(void)item;  // silence compiler warning re. unused variable
	
	// Always returns an invalid wxDataViewItem since none
	// of the items have parents that are not the root
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
		// Then the item is the root and all the AugmentedAccounts are its
		// children. Deliberately start counting from 1, as 0 is for an
		// invalid wxDataViewItem.
		return get_all_items(children);
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
	case 0:
		return wxString("string");
	case 1:
		return AccountTypeVariantData::GetTypeStatic();
	case 2:
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
	unsigned int* identifier_ptr = static_cast<unsigned int*>(item.GetID());
	unsigned int const identifier = *identifier_ptr;
	Map::const_iterator it = m_accounts.find(identifier);
	if (it == m_accounts.end())
	{
		throw AccountDataViewModelException
		(	"Could not find AugmentedAccount in AccountDataViewModel."
		);
	}
	AugmentedAccount const& aug_account = *(it->second);
	wxVariantData* data = 0;
	switch (col)
	{
	case 0:
		variant = bstring_to_wx(aug_account.account.name());
		break;
	case 1:
		data = new AccountTypeVariantData
		(	aug_account.account.account_type()
		);
		variant.SetData(data);
		break;
	case 2:
		data = new DecimalVariantData
		(	aug_account.technical_opening_balance
		);
		variant.SetData(data);
		break;
	default:
		throw AccountDataViewModelException("Invalid column number.");
	}
	return;
}

bool
AccountDataViewModel::SetValue
(	wxVariant const& variant,	
	wxDataViewItem const& item,
	unsigned int col
)
{
	unsigned int* identifier_ptr = static_cast<unsigned int*>(item.GetID());
	unsigned int const identifier = *identifier_ptr;
	Map::iterator it = m_accounts.find(identifier);
	if (it == m_accounts.end())
	{
		throw AccountDataViewModelException
		(	"Could not find AugmentedAccount in AccountDataViewModel."
		);
	}
	AugmentedAccount& aug_account = *(it->second);
	AccountTypeVariantData* atvd = 0;
	DecimalVariantData* dvd = 0;
	switch (col)
	{
	case 0:
		aug_account.account.set_name(wx_to_bstring(variant.GetString()));
		return true;
	case 1:
		atvd = dynamic_cast<AccountTypeVariantData*>
		(	variant.GetData()
		);
		if (!atvd)
		{
			throw AccountDataViewModelException("Wrong wxVariant type.");
		}
		aug_account.account.set_account_type(atvd->account_type());
		return true;
	case 2:
		dvd = dynamic_cast<DecimalVariantData*>
		(	variant.GetData()
		);
		if (!dvd)
		{
			throw AccountDataViewModelException("Wrong wxVariant type.");
		}
		aug_account.technical_opening_balance = dvd->decimal();
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
	(	Map::const_iterator it = m_accounts.begin(), end = m_accounts.end();
		it != end;
		++it
	)
	{
		void* identifier = new unsigned int(it->first);
		items.Add(wxDataViewItem(identifier));
	}
	return m_accounts.size();
}





}  // namespace gui
}  // namespace phatbooks
