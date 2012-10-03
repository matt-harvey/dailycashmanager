#!/usr/bin/tclsh8.5

# Execute this script to run the unit tests.

# This Tcl script is intended to drive the unit tests. We drive them from here
# rather than from directly from main, so that we can crash and then revive
# the main program, in the middle of a SQL transaction, and inspect the
# database afterwards, to see if the failed transaction was handled as
# expected. On reviving, the main function will run again, detect the
# existence of the crashed database file, check that the database state
# is as expected given the crash, output the result of this check to
# standard output, and then run the remaining unit tests.

# This one crashes, but we recover
catch { exec ./test } test_output_A
# And in this second execution we perform the test
catch { exec ./test } test_output_B
puts "Test output:\n\n $test_output_B \n\n"




