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

# Generates the ".desktop" file for Linux builds.

set req_args 4
if {$argc != $req_args} {
    error "Wrong number of arguments: should be $req_args."
}
lassign $argv application_name executable_name path_to_icon desktop_filename
set out [open $desktop_filename w]
puts $out {[Desktop Entry]}
puts $out ""
puts $out "\tValue=1.0"
puts $out "\tType=Application"
puts $out "\tName=${application_name}"
puts $out "\tGenericName=\"Personal budgeting application\""
puts $out "\tEncoding=UTF-8"
puts $out ""
puts $out "\tExec=${executable_name} %f"
puts $out "\tIcon=${path_to_icon}"
puts $out ""
flush $out
close $out
