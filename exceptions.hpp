#ifndef exceptions_hpp
#define exceptions_hpp


#include <stdexcept>
#include <string>

namespace phatbooks
{

class SQLiteException
{
public:
	SQLiteException(std::string p_message);
	~SQLiteException() throw();
	const char* what() throw();
private:
	std::string m_message;
};






}  // namespace phatbooks

#endif  // exceptions_hpp
