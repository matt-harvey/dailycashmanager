#ifndef GUARD_journal_hpp
#define GUARD_journal_hpp

namespace phatbooks
{

class Journal
{
private:
	enum Status
	{
		working,
		saved,
		posted
	}
	Status m_status;
	DateType m_date;	
	std::string m_comment;
	std::vector<IdType> entry_ids;
	IdType m_id;
};



}  // namespace phatbooks


#endif  // GUARD_journal_hpp
