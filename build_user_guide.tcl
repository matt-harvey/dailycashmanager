#!/usr/bin/env tclsh

###
# Copyright 2014 Matthew Harvey
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###

# Copy text of LICENSE into "user_guide" directory, with suitable title and
# file extension to satisfy Sphinx.
set in [open "LICENSE" r]
set out [open [file nativename "user_guide/LICENSE.rst"] w]
puts $out "LICENSE"
puts $out "======="
puts $out ""
while {[gets $in line] >= 0} {
    puts $out $line
}
close $in
flush $out
close $out

# Generate html user guide using Sphinx
exec sphinx-build -b html -d [file nativename "user_guide/_build/doctrees"] \
    user_guide [file nativename "user_guide/_build/html"] \
    2>@ stderr >@stdout

