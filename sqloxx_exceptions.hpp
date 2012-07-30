#ifndef sqloxx_exceptions_hpp
#define sqloxx_exceptions_hpp


#include <stdexcept>
#include <string>

namespace sqloxx
{


/**
 * @todo Make this into a proper hierarchy of exceptions, and use the
 * hierarchy judiciously throughout the Phatbooks code to facilitate
 * better diagnosis and handling of exceptions.
 */
class SQLiteException: public std::exception
{
public:
	explicit SQLiteException(std::string p_message);
	virtual ~SQLiteException() throw();
	virtual const char* what() const throw();
private:
	std::string m_message;
};






}  // namespace sqloxx

#endif  // sqloxx_exceptions_hpp
