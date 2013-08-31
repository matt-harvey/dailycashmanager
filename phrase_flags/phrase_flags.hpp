#ifndef GUARD_phrase_flags_hpp
#define GUARD_phrase_flags_hpp

#include <jewel/flag_set.hpp>

namespace phatbooks
{

/**
 * Contains flags to be used in bit manipulations
 * to indicate data about features of a natural
 * language phrase.
 */
namespace phrase_flags
{
	enum PhraseFlags
	{
		capitalize = (1 << 0),
		include_article = (1 << 1),
		pluralize = (1 << 2)
	};

}  // namespace phrase_flags


typedef
	jewel::FlagSet
	<	phrase_flags::PhraseFlags,
		phrase_flags::capitalize |
		phrase_flags::include_article |
		phrase_flags::pluralize
	>
	PhraseFlagSet;


}  // namespace phatbooks

#endif  // GUARD_phrase_flags_hpp
