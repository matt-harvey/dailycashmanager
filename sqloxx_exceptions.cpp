#include "sqloxx_exceptions.hpp"

#include <string>

namespace sqloxx
{


SQLiteException::SQLiteException(std::string p_message):
  m_message(p_message)
{
}

SQLiteException::~SQLiteException() throw()
{
}

const char*
SQLiteException::what() const throw()
{
	return m_message.c_str();
}




}  // namespace sqloxx




