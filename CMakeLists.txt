cmake_minimum_required (VERSION 2.6)

project (main)

project (test)

include_directories (/home/matthew/lib
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch
                     /home/matthew/Workbench/versioned/phatbooks_id_map_branch/account
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/commodity
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/date
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/entry
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/import_from_nap
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/journal
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/phatbooks_database_connection
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/phatbooks_text_session
					 /home/matthew/Workbench/versioned/phatbooks_id_map_branch/repeater)

link_directories (/home/matthew/lib/jewel)

set (main_files
	 commodity/commodity.cpp
	 commodity/commodity_impl.cpp
	 account/account.cpp
	 account/account_impl.cpp
	 consolixx/consolixx.cpp
	 date/date.cpp
	 entry/entry.cpp
	 import_from_nap/import_from_nap.cpp
	 journal/draft_journal.cpp
	 journal/journal.cpp
	 journal/ordinary_journal.cpp
	 sqloxx/database_connection.cpp
	 sqloxx/shared_sql_statement.cpp
	 sqloxx/detail/sqlite_dbconn.cpp
	 sqloxx/detail/sql_statement.cpp
	 phatbooks_database_connection/phatbooks_database_connection.cpp
	 phatbooks_text_session/phatbooks_text_session.cpp
	 repeater/repeater.cpp)

add_executable (main main.cpp ${main_files})

set (test_files
     test.cpp
	 sqloxx/database_connection.cpp
	 sqloxx/shared_sql_statement.cpp
	 sqloxx/detail/sqlite_dbconn.cpp
	 sqloxx/detail/sql_statement.cpp
     sqloxx/tests/database_connection_tests.cpp
	 sqloxx/tests/derived_po.cpp
	 sqloxx/tests/persistent_object_tests.cpp
	 sqloxx/tests/shared_sql_statement_tests.cpp
	 sqloxx/tests/sqloxx_tests_common.cpp
	 sqloxx/tests/atomicity_test.cpp)

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
					


