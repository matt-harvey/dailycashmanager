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
 * Abstract base class. A TextDialogue<Seed, Output> represents
 * a TUI interaction with the user. The objective of the dialogue
 * is to take an object of type Seed, get some valid input from the
 * user, and then produce an object of type Output. The object
 * should first be constructed by passing a Seed to the constructor.
 * Then generate() should be called to return the desired Output
 * object. The Input and Output objects should both be
 * copy-constructible.
 *
 * This is an abstract base class and the following pure virtual
 * methods need to be provided by a subclass.
 *
 * 		do_get_main_prompt() should be defined to return a std::string
 * 		that is the main prompt to be presented to the user, e.g.
 * 		"Please enter a number: ".
 *
 * 		do_validate_user_string(std::string const&) should be
 * 		defined to return true if and only if its string parameter
 * 		would be considered valid input from the user. If the
 * 		string is invalid, then an error message should generally
 * 		be printed to std::cout, within the body of
 * 		do_validate_user_string.
 *
 * 		do_transform(Seed const&, std::string const&) should be defined
 * 		to take a string - assumed to be already validated -
 * 		and a Seed, and produce the appropriate instance of Output.
 */
template <typename Seed, typename Output>
class TextDialogue
{
public:

	TextDialogue(Seed const& p_seed);
	Output generate() const;
	virtual ~TextDialogue();

protected:

	Seed seed() const;

private:
	
	virtual std::string do_get_main_prompt() const = 0;

	virtual bool do_validate_user_string(std::string const& s) const = 0;

	virtual Ouput do_transform
	(	Seed const& p_seed,
		std::string const& p_input
	) const = 0;
	
	std::string get_user_string() const;

	Seed const m_seed;
};


template <typename Seed, typename Output>
TextDialogue<Seed, Output>::TextDialogue(Seed const& p_seed):
	m_seed(p_seed)
{
}

template <typename Seed, typename Output>
TextDialogue<Seed, Output>::TextDialogue()
{
}

template <typename Seed, typename Output>
Seed
TextDialogue<Seed, Output>::seed() const
{
	return m_seed;
}

template <typename Seed, typename Output>
std::string
TextDialogue<Seed, Output>::get_user_string() const
{
	std::cout << do_get_main_prompt();
	return consolixx::get_user_input();
}

template <typename Seed, typename Output>
Output
TextDialogue<Seed, Output>::generate()
{
	std::string input;
	while (!do_validate_user_string(input = get_user_string()))
	{
	}
	return do_transform(m_seed, input);
}


}  // namespace phatbooks
}  // namespace tui

#endif  // GUARD_text_dialogue_hpp
