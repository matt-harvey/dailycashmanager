#!/usr/bin/env tclsh

###
# Copyright 2013 Matthew Harvey
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

# Scans currencies.csv and outputs the repetitive bit of code
# required for currency.cpp, to the output file passed to command line argument.
# It is intended the output file be #included by make_currencies.cpp.

set infile_name "currencies.csv"
set outfile_name [lindex $argv 0]

set infile [open $infile_name "r"]

# Discard comments and header row at top of file
set start_found 0
while {[gets $infile line] >= 0} {
    if {[string equal $line "START"]} {
        set start_found 1
        break
    }
}
if {!$start_found} {
    puts "Could not find \"START\" row while reading ${infile_name}."
    close $infile
    exit 1
}
gets $infile line
if {![string equal $line "Currency,Symbol,Precision"]} {
    puts "Could not find header row while reading ${infile_name}."
    close $infile
    exit 1
}

# Read content and populate outfile
set outfile [open $outfile_name "w"]
while {[gets $infile line] >= 0} {
	if {$line == {END}} {
		break
	}
	set record [split $line ","]
	puts -nonewline $outfile "\t\tvec.push_back(make_currency(dbc, "
	puts -nonewline $outfile "L\"[string trim [lindex $record 0]]\", "
	puts -nonewline $outfile "\"[string trim [lindex $record 1]]\", "
	puts $outfile "[string trim [lindex $record 2]]));"
}

flush $outfile
close $outfile
close $infile
