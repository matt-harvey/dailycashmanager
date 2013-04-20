#!/usr/bin/env tclsh

# Scans currencies.csv and outputs the repetitive bit of code
# required for currency.cpp, as file currency.inl. It is
# intended that currency.inl be #included in currency.cpp.
# (We don't output directly to currency.cpp because we don't
# want to clobber it!)

set infile [open currencies.csv r]
set outfile [open currency.inc w]

# Discard first line (headers)
gets $infile line

while {[gets $infile line]} {
	if {$line == {END}} {
		break
	}
	set record [split $line ,]
	puts -nonewline $outfile "\t\tcontainer.push_back(make_currency(dbc, "
	puts -nonewline $outfile "L\"[string trim [lindex $record 0]]\", "
	puts -nonewline $outfile "\"[string trim [lindex $record 1]]\", "
	puts $outfile "[string trim [lindex $record 2]]));"
}


flush $outfile
close $outfile
close $infile
