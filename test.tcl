#!/usr/bin/tclsh8.5

# This Tcl scrip is intended to drive the tests. We drive them from here
# rather than from directly from main, so that we can crash and then revive
# the main program, in the middle of a SQL transaction, and inspect the
# database afterwards, to see if the failed transaction was handled as
# expected.

load /usr/lib/tcltk/sqlite3/libtclsqlite3.so
package require sqlite3

catch { exec ./test } test_output_A

puts "Test output:\n\n $test_output_A \n\n"

catch { exec ./test } test_output_B

puts "More test output:\n\n $test_output_B \n\n"




