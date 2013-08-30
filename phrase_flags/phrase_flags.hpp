#ifndef GUARD_phrase_flags_hpp
#define GUARD_phrase_flags_hpp

namespace phatbooks
{

/**
 * Contains flags to be used in bit manipulations
 * to indicate data about features of a natural
 * language phrase.
 */
namespace phrase_flags
{
	typedef unsigned int PhraseFlags;

	enum
	{
		none = 0,
		capitalize = (1 << 0),
		include_article = (1 << 1),
		pluralize = (1 << 2)
	};

}  // namespace phrase_flags

}  // namespace phatbooks

#endif  // GUARD_phrase_flags_hpp
