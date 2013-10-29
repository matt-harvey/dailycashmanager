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

#include "finformat.hpp"
#include <jewel/decimal.hpp>
#include <wx/app.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <UnitTest++/UnitTest++.h>

using jewel::Decimal;

namespace phatbooks
{
namespace test
{

struct FinformatTestFixture
{
	FinformatTestFixture()
	{
		// We need this only to make sure we can use wxLocale.
		wxApp* app = new wxApp;

		(void)app;  // silence compiler warning re. unused parameter
		loc.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT);
	}
	wxLocale loc;
};

TEST_FIXTURE(FinformatTestFixture, test_finformat_wx)
{
	// TODO HIGH PRIORITY Write these tests, and make sure they're
	// locale-neutral.
	Decimal const d0(3001, 3);
	wxString const wx0 = finformat_wx(d0, loc);
}

TEST_FIXTURE(FinformatTestFixture, test_wx_to_decimal)
{
	// TODO HIGH PRIORITY Write these tests, and make sure they're
	// locale-neutral.
}

TEST_FIXTURE(FinformatTestFixture, test_wx_to_simple_sum)
{
	// TODO HIGH PRIORITY Write these tests, and make sure they're
	// locale-neutral.
}




}  // namespace test
}  // namespace phatbooks
