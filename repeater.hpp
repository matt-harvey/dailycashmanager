#ifndef GUARD_repeater_hpp
#define GUARD_repeater_hpp

namespace phatbooks
{

class Repeater
{
private:
	enum IntervalScale
	{
		days,
		weeks,
		months,
		month_ends
	};
	IntervalScale m_interval_scale;
	int m_interval_units;
	DateType m_next_date_due;
	IdType m_journal_id;
	IdType m_id;
};




}  // namespace phatbooks

#endif  // GUARD_repeater_hpp
