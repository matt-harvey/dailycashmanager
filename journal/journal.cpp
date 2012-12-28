#include "journal.hpp"
#include "entry.hpp"
#include <jewel/decimal.hpp>
#include <string>
#include <vector>

using jewel::Decimal;
using std::string;
using std::vector;

namespace phatbooks
{

Journal::~Journal()
{
}

void
Journal::set_whether_actual(bool p_is_actual)
{
	do_set_whether_actual(p_is_actual);
	return;
}

void
Journal::set_comment(string const& p_comment)
{
	do_set_comment(p_comment);
	return;
}

void
Journal::add_entry(Entry& entry)
{
	do_add_entry(entry);
	return;
}

std::vector<Entry> const&
Journal::entries() const
{
	return do_get_entries();
}

string
Journal::comment() const
{
	return do_get_comment();
}

bool
Journal::is_actual() const
{
	return do_get_whether_actual();
}

Decimal
Journal::balance() const
{
	typedef vector<Entry>::const_iterator Iter;
	Decimal ret(0, 0);
	for (Iter it = entries().begin(), end = entries().end(); it != end; ++it)
	{
		ret += it->amount();
	}
	return ret;
}
		

bool
Journal::is_balanced() const
{
	return balance() == Decimal(0, 0);
}

		

}  // namespace phatbooks
