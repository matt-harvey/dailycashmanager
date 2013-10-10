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


#include "phatbooks_exceptions.hpp"
#include "repeater.hpp"
#include "session.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <sqloxx/sqloxx_exceptions.hpp>
#include <string>

using jewel::Log;
using sqloxx::InvalidFilename;
using std::string;

namespace gregorian = boost::gregorian;

namespace phatbooks
{

int Session::s_num_instances = 0;

// TODO This is ignoring the PhatbooksDatabaseConnection which we
// have ALREADY created as part of the Session constructor.
// The whole Session hierarchy, and the interaction between
// GraphicalSession and App, is a bit of a mess.
Session::Session()
{
	++s_num_instances;
	if (s_num_instances > s_max_instances)
	{
		--s_num_instances;
		JEWEL_THROW
		(	TooManySessions,
			"Exceeded maximum number of instances of phatbooks::Session."
		);
	}
	JEWEL_ASSERT (s_num_instances <= s_max_instances);
}


Session::~Session()
{
	JEWEL_ASSERT (s_num_instances > 0);
	JEWEL_ASSERT (s_num_instances <= s_max_instances);
	--s_num_instances;
}


int
Session::run()
{
	return do_run();
}


int
Session::run(string const& filepath_str)
{
	if (filepath_str.empty())
	{
		JEWEL_THROW(InvalidFilename, "Filename is empty string.");
	}	
	return do_run(filepath_str);
}




}  // namespace phatbooks
