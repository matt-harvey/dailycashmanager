#ifndef exceptions_hpp
#define exceptions_hpp


#include <stdexcept>
#include <string>

namespace phatbooks
{

class SQLiteException: public std::exception
{
public:
	explicit SQLiteException(std::string p_message);
	virtual ~SQLiteException() throw();
	virtual const char* what() const throw();
private:
	std::string m_message;
};






}  // namespace phatbooks

#endif  // exceptions_hpp
