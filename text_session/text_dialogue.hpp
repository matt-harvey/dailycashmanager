// Copyright (c) 2013, Matthew Harvey. All rights reserved.

#ifndef GUARD_text_dialogue_hpp
#define GUARD_text_dialogue_hpp

#include <consolixx/get_input.hpp>
#include <iostream>
#include <string>

namespace phatbooks
{
namespace tui
{


/**
 * Abstract base class. A TextDialogue<Target> represents
 * a TUI interaction with the user. The objective of the dialogue
 * is to take an non-const ref to an object of type Target, get
 * some valid input from the
 * user, and then perform transformations on the Target according to
 * the input. The object
 * should first be constructed by passing a Target& to the constructor.
 * Then process() should be called to perform the transformation.
 *
 * This is an abstract base class and the following pure virtual
 * methods need to be defined by a subclass.
 *
 * 		do_create_main_prompt() should be defined to return a std::string
 * 		that is the main prompt to be presented to the user, e.g.
 * 		"Please enter a number: ".
 *
 * 		do_validate_user_string(std::string const&) should be
 * 		defined to return true if and only if its string parameter
 * 		would be considered valid input from the user. If the
 * 		string is invalid, then an error message should generally
 * 		be printed to std::cout, within the body of
 * 		do_validate_user_string, prior to returning false.
 *
 * 		do_transform(std::string const&) should be defined
 * 		to take a string - assumed to be already validated -
 * 		and to perform whatever transformations are desired on
 * 		the Target&. Note the subclass can access the Target
 * 		using the protected function target(), which returns
 * 		a non-const reference to the Target, enabling arbitrary
 * 		transformations to be performed to the Target from the
 * 		subclass.
 */
template <typename Target>
class TextDialogue
{
public:

	TextDialogue(Target& p_target);
	void process() const;
	virtual ~TextDialogue();

protected:

	Target& target();

private:
	
	virtual std::string do_create_main_prompt() const = 0;

	virtual bool do_validate_user_string(std::string const& s) const = 0;

	virtual void do_process(std::string const& p_input) = 0;
	
	std::string get_user_string() const;

	Target& m_target;
};


template <typename Target>
TextDialogue<Target>::TextDialogue(Target const& p_target):
	m_target(p_target)
{
}

template <typename Target>
TextDialogue<Target>::TextDialogue()
{
}

template <typename Target>
Target
TextDialogue<Target>::target() const
{
	return m_target;
}

template <typename Target>
std::string
TextDialogue<Target>::get_user_string() const
{
	std::cout << do_create_main_prompt();
	return consolixx::get_user_input();
}

template <typename Target>
Output
TextDialogue<Target>::process()
{
	std::string input;
	while (!do_validate_user_string(input = get_user_string()))
	{
	}
	return do_transform(input);
}


}  // namespace phatbooks
}  // namespace tui

#endif  // GUARD_text_dialogue_hpp
