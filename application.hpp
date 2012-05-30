#ifndef GUARD_application_hpp
#define GUARD_application_hpp

#include <iostream>

class Application
{
public:
	void execute();
	void run_opening_tests();
	void run_closing_tests();
protected:
private:
};

inline void Application::execute()
{
	std::cout << "Executing dummy application..." << std::endl;
	return;
}

inline void Application::run_opening_tests()
{
	std::cout << "Running dummy application opening tests..."
	          << std::endl;
	return;
}

inline void Application::run_closing_tests()
{
	std::cout << "Running dummy application closing tests..."
	          << std::endl;
	return;
}

#endif
