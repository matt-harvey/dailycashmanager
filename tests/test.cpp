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

#include "application.hpp"
#include "string_conv.hpp"
#include <UnitTest++/UnitTest++.h>
#include <jewel/log.hpp>
#include <string>

using jewel::Log;
using phatbooks::Application;
using phatbooks::wx_to_std8;
using std::string;

void configure_logging()
{
	Log::set_threshold(Log::trace);
	string const log_name = 
		 "./" + wx_to_std8(Application::application_name()) + "_test.log";
	Log::set_filepath(log_name);
	return;
}


int main()
{
	configure_logging();
	try
	{
		return UnitTest::RunAllTests();
	}
	// This seems pointless but is necessary to guarantee the stack is
	// fully unwound if an exception is thrown.
	catch (...)
	{
		throw;
	}
}
