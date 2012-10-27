#!/usr/bin/env tclsh

# Execute this script to run the unit tests.

# This Tcl script is intended to drive the unit tests. We drive them from here
# rather than directly from main, so that we can crash and then revive
# the main program, in the middle of a SQL transaction, and inspect the
# database afterwards, to see if the failed transaction was handled as
# expected. On reviving, the main function will run again, detect the
# existence of the crashed database file, check that the database state
# is as expected given the crash, output the result of this check to
# standard output, and then run the remaining unit tests.
#
# As a convenience, this script will also execute "make test" before
# running the tests.

puts "Building unit tests...\n"

set make_result [catch { exec make test 2>@ stderr >@ stdout } ]
if { $make_result != 0 } {
	exit $make_result
}
puts "\nRunning unit tests..."

set filename testfile9182734123.db

if {[file exists $filename] || [file exists ${filename}-journal]} {
	puts "File named $filename and/or ${filename}-journal already exists. "
	puts "Test aborted as unsafe to proceed."
	exit 1
}

# This execution crashes, but we recover
catch { exec ./test $filename 2>@ stderr >@ stdout }

# And in this second execution we inspect the database to see that it
# reacted as expected; and then we perform the other unit tests.
catch { exec ./test $filename 2>@ stderr >@ stdout }

# And clean up left over files
catch { file delete $filename }
catch { file delete ${filename}-journal }




