#ifndef GUARD_session_hpp
#define GUARD_session_hpp

#include "entity.hpp"
#include <iostream>

namespace phatbooks
{

class Session
{
public:
	void execute();
	void run_opening_tests();
	void run_closing_tests();
protected:
private:
	Entity m_entity;
};

inline void Session::execute()
{
	std::cout << "Executing dummy session..." << std::endl;
	return;
}

inline void Session::run_opening_tests()
{
	std::cout << "Running dummy session opening tests..."
	          << std::endl;
	return;
}

inline void Session::run_closing_tests()
{
	std::cout << "Running dummy session closing tests..."
	          << std::endl;
	return;
}


}  // namespace phatbooks

#endif  // GUARD_session_hpp
