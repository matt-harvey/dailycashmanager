#ifndef GUARD_identifier_hpp
#define GUARD_identifier_hpp

#include "database.hpp"
#include <string>

class Identifier
{
private:
	std::string m_str_rep;
	Database m_storage_location;
};

#endif
