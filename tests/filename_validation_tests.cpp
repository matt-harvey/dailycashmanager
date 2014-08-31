/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "filename_validation.hpp"
#include <boost/test/unit_test.hpp>
#include <jewel/assert.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace dcm
{
namespace test
{

BOOST_AUTO_TEST_CASE(test_is_valid_filename_re_generally_bad_filenames)
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
		"even_with_extension?.dcm",
		"that_won>t_save_you.dcm"
	};
	for (char const* bad_name: generally_bad_filenames)
	{
		string message;
		BOOST_CHECK(!is_valid_filename(bad_name, message));
		BOOST_CHECK(!is_valid_filename(bad_name, message, true));
		BOOST_CHECK(!is_valid_filename(bad_name, message, false));
	}

	// We have to test strings containing the null character separately, as
	// we include the null character in a string literals as above.
	string nul_containing_string;
	nul_containing_string.push_back('\0');
	string message;
	BOOST_CHECK(!is_valid_filename(nul_containing_string, message));
	BOOST_CHECK(!is_valid_filename(nul_containing_string, message, true));
	BOOST_CHECK(!is_valid_filename(nul_containing_string, message, false));
	string nul_containing_string_2;
	nul_containing_string_2.push_back('a');
	nul_containing_string_2.push_back('b');
	nul_containing_string_2.push_back('\0');
	nul_containing_string_2.push_back('c');
	JEWEL_ASSERT (nul_containing_string_2.size() == 4);
	BOOST_CHECK(!is_valid_filename(nul_containing_string_2, message));
	BOOST_CHECK(!is_valid_filename(nul_containing_string_2, message, true));
	BOOST_CHECK(!is_valid_filename(nul_containing_string_2, message, false));
}



BOOST_AUTO_TEST_CASE(test_is_valid_filename_re_generally_good_filenames)
{
	// Note none of these have the right DailyCashManager extension though.
	// We test that they are good generally filenames but can't
	// serve as DailyCashManager database files.
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
		"does_not_really_have_a.dcm.extension",
		".dcm",   // Must have non-empty base
		"nodotdcm",
		"badly_positioned_do.tdcm",
		"so_is_thisp.hat",
		"and_this.dcm."
	};
	for (char const* good_name: generally_good_filenames)
	{
		string message;

		// Good generally
		BOOST_CHECK(is_valid_filename(good_name, message, false));
		if (!is_valid_filename(good_name, message, false))
		{
			cout << good_name << endl;
		}

		// But doesn't have DailyCashManager extension
		BOOST_CHECK(!is_valid_filename(good_name, message, true));
		BOOST_CHECK(!is_valid_filename(good_name, message));
	}
}


BOOST_AUTO_TEST_CASE(test_is_valid_filename_re_good_dcm_filenames)
{
	const char* const good_dcm_filenames[] =
	{	"hello.dcm",
		"space are ok.dcm",
		"this is weird     .dcm",
		"------.dcm",
		"'.dcm",
		"we-are-tolerant-of-hyphens.dcm"
	};
	for (char const* good_name: good_dcm_filenames)
	{
		string message;

		// Good generally
		BOOST_CHECK(is_valid_filename(good_name, message, false));

		// Good as DailyCashManager file, too
		BOOST_CHECK(is_valid_filename(good_name, message, true));
		BOOST_CHECK(is_valid_filename);
	}
}


}  // namespace test
}  // namespace dcm
