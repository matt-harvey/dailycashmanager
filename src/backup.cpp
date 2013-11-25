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

#include "backup.hpp"
#include "date.hpp"
#include "dcm_exceptions.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <jewel/assert.hpp>
#include <jewel/exception.hpp>
#include <jewel/log.hpp>
#include <string>
#include <sstream>

using std::ostringstream;
using std::string;

namespace filesystem = boost::filesystem;
namespace posix_time = boost::posix_time;

namespace dcm
{

filesystem::path
make_backup
(	filesystem::path const& p_original,
	filesystem::path const& p_directory,
	string const& p_infix
)
{
	JEWEL_LOG_TRACE();

	// Assert preconditions
	JEWEL_ASSERT (filesystem::exists(p_original));
	JEWEL_ASSERT (filesystem::exists(p_directory));
	JEWEL_ASSERT
	(	filesystem::absolute(p_original) ==
		p_original
	);
	JEWEL_ASSERT
	(	filesystem::absolute(p_directory) ==
		p_directory
	);

	auto const original_filename = p_original.filename();
	string const original_filename_str = original_filename.string();
	auto const begin = original_filename_str.begin();
	auto const end = original_filename_str.end();
	auto it = end;
	while ((*it != '.') && (it != begin)) --it;
	for (unsigned short i = 0; i != USHRT_MAX; ++i)
	{
		ostringstream oss;
		oss << string(begin, it)
			<< p_infix
			<< "-"
			<< posix_time::to_iso_string(now());
		if (i > 1) oss << "-" << i;
		if (it != end) oss << string(it, end);
		filesystem::path const new_filename(oss.str());
		filesystem::path const new_filepath = p_directory / new_filename;
		if (!filesystem::exists(new_filepath))
		{
			filesystem::copy(p_original, new_filepath);
			JEWEL_LOG_TRACE();
			JEWEL_ASSERT
			(	filesystem::absolute(new_filepath) ==
				new_filepath
			);
			return new_filepath;
		}
	}
	JEWEL_THROW
	(	UniqueNameException,
		"Could not generate unique name for backup file."
	);
}

}  // namespace dcm
