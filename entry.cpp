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
	string p_comment,
	Decimal p_amount,
	bool p_is_actual
):
	m_journal(p_journal),
	m_comment(p_comment),
	m_amount(p_amount),
	m_is_actual(p_is_actual)
{
}




}  // namespace phatbooks
