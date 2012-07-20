#include "exceptions.hpp"

#include <string>

namespace phatbooks
{


SQLiteException::SQLiteException(std::string p_message):
  m_message(p_message)
{
}

SQLiteException::~SQLiteException() throw()
{
}

const char*
SQLiteException::what() throw()
{
	return m_message.c_str();
}




}  // namespace phatbooks


