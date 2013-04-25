#include "account_data_view_model.hpp"
#include "account.hpp"
#include "account_type_variant_data.hpp"
#include "decimal_variant_data.hpp"
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
		assert (false);
	}
}

void
AccountDataViewModel::GetValue
(	wxVariant& variant,
	wxDataViewItem const& item,
	unsigned int col
) const
{
	// TODO Implement
}

bool
AccountDataViewModel::SetValue
(	wxVariant const& variant,	
	wxDataViewItem const& item,
	unsigned int col
)
{
	// TODO Implement
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
