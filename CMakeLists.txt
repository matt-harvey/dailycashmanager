cmake_minimum_required (VERSION 2.6)

project (main)

project (test)

include_directories (/home/matthew/lib
					 /home/matthew/Workbench/versioned/phatbooks
                     /home/matthew/Workbench/versioned/phatbooks/account
                     /home/matthew/Workbench/versioned/phatbooks/commodity
                     /home/matthew/Workbench/versioned/phatbooks/date
                     /home/matthew/Workbench/versioned/phatbooks/entry
                     /home/matthew/Workbench/versioned/phatbooks/import_from_nap
                     /home/matthew/Workbench/versioned/phatbooks/journal
                     /home/matthew/Workbench/versioned/phatbooks/phatbooks_database_connection
                     /home/matthew/Workbench/versioned/phatbooks/phatbooks_text_session
                     /home/matthew/Workbench/versioned/phatbooks/repeater)

link_directories (/home/matthew/lib/jewel)

set (main_files
	 commodity/commodity.cpp
	 commodity/commodity_impl.cpp
	 account/account.cpp
	 account/account_impl.cpp
	 consolixx/consolixx.cpp
	 date/date.cpp
	 entry/entry.cpp
	 entry/entry_impl.cpp
	 import_from_nap/import_from_nap.cpp
	 journal/draft_journal.cpp
	 journal/draft_journal_impl.cpp
	 journal/journal.cpp
	 journal/ordinary_journal.cpp
	 journal/ordinary_journal_impl.cpp
	 sqloxx/database_connection.cpp
	 sqloxx/database_transaction.cpp
	 sqloxx/reader.cpp
	 sqloxx/sql_statement.cpp
	 sqloxx/detail/sqlite_dbconn.cpp
	 sqloxx/detail/sql_statement_impl.cpp
	 phatbooks_database_connection/phatbooks_database_connection.cpp
	 phatbooks_text_session/phatbooks_text_session.cpp
	 repeater/repeater.cpp
	 repeater/repeater_impl.cpp)

add_executable (main main.cpp ${main_files})

set (test_files
     test.cpp
	 sqloxx/database_connection.cpp
	 sqloxx/database_transaction.cpp
	 sqloxx/reader.cpp
	 sqloxx/sql_statement.cpp
	 sqloxx/detail/sqlite_dbconn.cpp
	 sqloxx/detail/sql_statement_impl.cpp
     sqloxx/tests/database_connection_tests.cpp
	 sqloxx/tests/derived_po.cpp
	 sqloxx/tests/persistent_object_tests.cpp
	 sqloxx/tests/sql_statement_tests.cpp
	 sqloxx/tests/sqloxx_tests_common.cpp
	 sqloxx/tests/atomicity_test.cpp
	 sqloxx/tests/get_handle_tests.cpp
	 sqloxx/tests/database_transaction_tests.cpp
	 sqloxx/tests/handle_tests.cpp
	 sqloxx/tests/identity_map_tests.cpp
	 sqloxx/tests/next_auto_key_tests.cpp
	 sqloxx/tests/reader_tests.cpp)

add_executable (test ${test_files})

set (libraries
     boost_date_time
	 boost_filesystem
	 boost_regex
	 boost_system
	 unittest++
	 jewel
	 sqlite3)

target_link_libraries (main ${libraries})

target_link_libraries (test ${libraries})
					


