#include "filename_validation.cpp"
#include <UnitTest++/UnitTest++.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace phatbooks
{
namespace test
{

TEST(test_is_valid_filename_re_generally_bad_filenames)
{
	const char* const generally_bad_filenames[] =
	{	"\\",
		"multiple_bad_chars<are_here_:yes",
		"another_bad_char\"yes",
		"question_?mark",
		"con",
		"CON",
		"com1",
		"COM8",
		"lpt9",
		"AUX",
		"nul",
		"slashes/are/us",
		"hmmm\\",
		"::",
		"and*yeah",
		"...",
		"",
		"..",
		"consecutive_dots_are..uncool",
		"should_not\\be_confusable\\with_a_Windows_filepath",
		"or/a/Unix/one",
		"/or/this/one",
		"D:\\this\\is\\not\\a_filename",
		"uncool/",
		"even_with_extension?.phat",
		"that_won>t_save_you.phat"
	};
	std::size_t const num_generally_bad_filenames =
		sizeof(generally_bad_filenames) / sizeof(generally_bad_filenames[0]);
	for (std::size_t i = 0; i != num_generally_bad_filenames; ++i)
	{
		string const bad_name(generally_bad_filenames[i]);
		string message;
		CHECK(!is_valid_filename(bad_name, message));
		CHECK(!is_valid_filename(bad_name, message, true));
		CHECK(!is_valid_filename(bad_name, message, false));
	}

	// We have to test strings containing the null character separately, as
	// we include the null character in a string literals as above.
	string nul_containing_string;
	nul_containing_string.push_back('\0');
	string message;
	CHECK(!is_valid_filename(nul_containing_string, message));
	CHECK(!is_valid_filename(nul_containing_string, message, true));
	CHECK(!is_valid_filename(nul_containing_string, message, false));
	string nul_containing_string_2;
	nul_containing_string_2.push_back('a');
	nul_containing_string_2.push_back('b');
	nul_containing_string_2.push_back('\0');
	nul_containing_string_2.push_back('c');
	assert (nul_containing_string_2.size() == 4);
	CHECK(!is_valid_filename(nul_containing_string_2, message));
	CHECK(!is_valid_filename(nul_containing_string_2, message, true));
	CHECK(!is_valid_filename(nul_containing_string_2, message, false));
}



TEST(test_is_valid_filename_re_generally_good_filenames)
{
	// Note none of these have the right Phatbooks extension though.
	// We test that they are good generally filenames but can't
	// serve as Phatbooks database files.
	const char* const generally_good_filenames[] =
	{	"hello",
		"something.with.lots.of.dots",
		"  leading spaces",
		"exclamation_marks!!",
		" ! ! ! ,,,",
		"all_good_here",
		"bonus_extension",
		"and.db",
		"but_extension_not_required",
		"HOW_ARE_CAPITALS_EVEN_AN_ISSUE",
		"spaces are fine          even    stupidly many  ",
		"does_not_really_have_a.phat.extension",
		".phat",   // Must have non-empty base
		"nodotphat",
		"badly_positioned_do.tphat",
		"so_is_thisp.hat",
		"and_this.phat."
	};
	std::size_t const num_generally_good_filenames =
		sizeof(generally_good_filenames) /
		sizeof(generally_good_filenames[0]);
	for (std::size_t i = 0; i != num_generally_good_filenames; ++i)
	{
		string const good_name(generally_good_filenames[i]);
		string message;

		// Good generally
		CHECK(is_valid_filename(good_name, message, false));

		// But doesn't have Phatbooks extension
		CHECK(!is_valid_filename(good_name, message, true));
		CHECK(!is_valid_filename(good_name, message));
	}
}


TEST(test_is_valid_filename_re_good_phatbooks_filenames)
{
	const char* const good_phatbooks_filenames[] =
	{	"hello.phat",
		"space are ok.phat",
		"this is weird     .phat",
		"------.phat",
		"'.phat",
		"we-are-tolerant-of-hyphens.phat"
	};
	std::size_t const num_good_phatbooks_filenames =
		sizeof(good_phatbooks_filenames) /
		sizeof(good_phatbooks_filenames[0]);
	for (std::size_t i = 0; i != num_good_phatbooks_filenames; ++i)
	{
		string const good_name(good_phatbooks_filenames[i]);
		string message;

		// Good generally
		CHECK(is_valid_filename(good_name, message, false));

		// Good as Phatbooks file, too
		CHECK(is_valid_filename(good_name, message, true));
		CHECK(is_valid_filename);
	}
}


}  // namespace test
}  // namespace phatbooks
