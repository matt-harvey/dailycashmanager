#ifndef GUARD_identifier_hpp
#define GUARD_identifier_hpp

#include "database.hpp"
#include <string>

class Identifier
{
private:
	std::string str_rep_;
	Database storage_location_;
};

#endif
