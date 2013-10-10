/*
 * This file is part of the Phatbooks project and is distributed under the
 * terms of the license contained in the file LICENSE.txt distributed
 * with this package.
 * 
 * Author: Matthew Harvey <matthew@matthewharvey.net>
 *
 * Copyright (c) 2012-2013, Matthew Harvey.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GUARD_phatbooks_tests_common_hpp_7922174706087529
#define GUARD_phatbooks_tests_common_hpp_7922174706087529

#include "phatbooks_database_connection.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

namespace phatbooks
{
namespace test
{


bool file_exists(boost::filesystem::path const& filepath);
void abort_if_exists(boost::filesystem::path const& dbc);
void setup_test_commodities(PhatbooksDatabaseConnection& dbc);
void setup_test_accounts(PhatbooksDatabaseConnection& dbc);

struct TestFixture
{
	// Setup
	TestFixture();

	// Teardown
	~TestFixture();

	// Database filepath
	boost::filesystem::path db_filepath;

	// The connection to the database
	PhatbooksDatabaseConnection* pdbc;
};








}  // namespace test
}  // namespace phatbooks


#endif  // GUARD_phatbooks_tests_common_hpp_7922174706087529
