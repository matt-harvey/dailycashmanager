#!/usr/bin/env tclsh

# Scans currencies.csv and outputs the repetitive bit of code
# required for currency.cpp, as file make_currencies_inc.hpp. It is
# intended make_currencies_inc.hpp be #included in make_currencies.cpp.

set infile [open "currencies.csv" "r"]
set outfile [open [file join "include" "make_currencies_inc.hpp"] "w"]

# Discard first line (headers)
gets $infile line

while {[gets $infile line] > 0} {
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
