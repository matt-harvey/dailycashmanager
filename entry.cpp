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
(	shared_ptr<Journal> p_journal,
	string const& p_comment,
	Decimal const& p_amount
):
	m_journal(p_journal),
	m_comment(p_comment),
	m_amount(p_amount)
{
}




}  // namespace phatbooks
