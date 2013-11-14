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

#include "app.hpp"
#include "string_conv.hpp"
#include <UnitTest++/UnitTest++.h>
#include <jewel/log.hpp>
#include <string>

using jewel::Log;
using phatbooks::App;
using phatbooks::wx_to_std8;
using std::string;

void configure_logging()
{
	Log::set_threshold(Log::trace);
	string const log_name =
		"./" + wx_to_std8(App::application_name()) + "_test.log";
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
