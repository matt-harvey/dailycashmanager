#include "entry.hpp"
#include <boost/shared_ptr.hpp>
#include <jewel/decimal.hpp>
#include <string>

using boost::shared_ptr;
using jewel::Decimal;
using std::string;

namespace phatbooks
{

Entry::Entry
(	string const& p_account_name,
	string const& p_comment,
	Decimal const& p_amount
):
	m_account_name(p_account_name),
	m_comment(p_comment),
	m_amount(p_amount)
{
}




}  // namespace phatbooks
