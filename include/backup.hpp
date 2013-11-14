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

#ifndef GUARD_backup_hpp_7859126994320716
#define GUARD_backup_hpp_7859126994320716

#include <boost/filesystem.hpp>

namespace phatbooks
{

/**
 * Takes a copy of file at \e p_original and saves it in \e p_directory, with
 * a name based on the name of \e p_original, in accordance with the following
 * examples:
 *
 * If \e p_original is "something.xyz", and the backup was made at 1:05pm, local
 * time, on 10 May 2016, then the backup will be named
 * "something-backup-20130510T1305.xyz". If there is already a file with this
 * name at the given location, then it will be named
 * "something-backup-20130510T1305-2.xyz". If there is already a file at \e this
 * location, then it will be named "something-backup-20130510-3.xyz". Etc.
 * The final number (e.g. "-3") that is appended to avoid duplicates, will never
 * be larger than SHRT_MAX.
 *
 * @returns the absolute filepath corresponding to the saved location
 *
 * @throws std::bad_alloc, in the unlikely event of memory allocation failure.
 *
 * @throws phatbooks::UniqueNameException if there are already SHRT_MAX
 * backups saved in the requested location with the requested "base
 * name" (\e extremely unlikely, as the "base name" for these purposes includes
 * the suffix indicating date and time).
 *
 * @throws boost::filesystem::filesystem_error, if the copy operation fails for
 * some reason other than memory allocation failure.
 *
 * Preconditions: it is the caller's resonsibility to ensure that
 * p_directory and p_original exist. Also, both p_original and p_directory
 * should be absolute filepaths.
 */
boost::filesystem::path make_backup
(	boost::filesystem::path const& p_original,
	boost::filesystem::path const& p_directory
);

}  // namespace phatbooks

#endif  // GUARD_backup_hpp_7859126994320716
