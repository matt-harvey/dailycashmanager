cmake_minimum_required (VERSION 2.6)

project (main)
project (test)

include_directories (/home/matthew/lib
					 /home/matthew/Workbench/versioned/phatbooks
                     /home/matthew/Workbench/versioned/phatbooks/account
                     /home/matthew/Workbench/versioned/phatbooks/commodity
                     /home/matthew/Workbench/versioned/phatbooks/date
                     /home/matthew/Workbench/versioned/phatbooks/entry
                     /home/matthew/Workbench/versioned/phatbooks/finformat
                     /home/matthew/Workbench/versioned/phatbooks/import_from_nap
                     /home/matthew/Workbench/versioned/phatbooks/journal
                     /home/matthew/Workbench/versioned/phatbooks/phatbooks_database_connection
                     /home/matthew/Workbench/versioned/phatbooks/phatbooks_text_session
                     /home/matthew/Workbench/versioned/phatbooks/repeater
					 /home/matthew/Workbench/versioned/phatbooks/session)

link_directories (/home/matthew/lib/jewel
                  /home/matthew/lib/sqloxx)

set (main_files
	 account/account.cpp
	 account/account_impl.cpp
	 account/account_reader.cpp
	 account/balance_cache.cpp
	 commodity/commodity.cpp
	 commodity/commodity_impl.cpp
	 consolixx/get_input.cpp
	 consolixx/table.cpp
	 consolixx/text_session.cpp
	 date/date.cpp
	 entry/entry.cpp
	 entry/entry_impl.cpp
	 entry/entry_reader.cpp
	 finformat/finformat.cpp
	 import_from_nap/import_from_nap.cpp
	 journal/draft_journal.cpp
	 journal/draft_journal_impl.cpp
	 journal/draft_journal_reader.cpp
	 journal/journal.cpp
	 journal/ordinary_journal.cpp
	 journal/ordinary_journal_impl.cpp
	 journal/ordinary_journal_reader.cpp
	 session/session.cpp
	 phatbooks_database_connection/phatbooks_database_connection.cpp
	 phatbooks_text_session/phatbooks_text_session.cpp
	 repeater/interval_type.cpp
	 repeater/repeater.cpp
	 repeater/repeater_impl.cpp
	 repeater/repeater_reader.cpp)

set (test_files
	 ${main_files}
	 tests/test.cpp
	 tests/interval_type_tests.cpp
	 tests/ordinary_journal_tests.cpp
	 tests/phatbooks_tests_common.cpp
	 tests/repeater_tests.cpp)

add_executable (main main.cpp ${main_files})
add_executable (test ${test_files})

set (libraries
     boost_date_time
	 boost_filesystem
	 boost_locale-mt
	 boost_regex
	 boost_system
	 unittest++
	 jewel
	 sqlite3
	 sqloxx)

target_link_libraries (main ${libraries})
target_link_libraries (test ${libraries})
					


