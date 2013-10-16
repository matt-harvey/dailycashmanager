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

// TODO MEDIUM PRIORITY The whole Session hierarchy, and the interaction between
// GraphicalSession and App, is needlessly complex, if we consider
// that we no longer have a TUI interface, and the only class derived from
// Session is now GraphicalSession. If we abandon the "wxWidgets-external"
// application / initialization stuff, then we could abolish Session and
// GraphicalSession classes, just use gui::App (inheriting from wxApp),
// probably also abolish the confusingly-named phatbooks::Application class,
// and significantly simplify the initialization code
// that is currently spread between Session, GraphicalSession and App.
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
