cmake_minimum_required (VERSION 2.6)

project (phatbooks)
project (phatbooks_test)

set (immediate_directories
     .
	 account
	 commodity
	 date
	 entry
	 finformat
	 import_from_nap
	 journal
	 output_aux
	 phatbooks_database_connection
	 phatbooks_persistent_object
	 phatbooks_text_session
	 repeater
	 session)

if (MSYS)
	include_directories(${immediate_directories} C:/MSYS/1.0/local/include)
	link_directories(C:/MSYS/1.0/lib C:/MSYS/1.0/local/lib)
else ()
	include_directories(${immediate_directories})
	link_directories(/usr/local/lib)
endif ()


set (main_files
	 account/account.cpp
	 account/account_impl.cpp
	 account/account_reader.cpp
	 account/account_type.cpp
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
	 journal/proto_journal.cpp
	 journal/ordinary_journal.cpp
	 journal/ordinary_journal_impl.cpp
	 journal/ordinary_journal_reader.cpp
	 journal/persistent_journal.cpp
	 session/session.cpp
	 phatbooks_database_connection/phatbooks_database_connection.cpp
	 phatbooks_persistent_object/phatbooks_persistent_object.cpp
	 phatbooks_text_session/phatbooks_text_session.cpp
	 repeater/interval_type.cpp
	 repeater/repeater.cpp
	 repeater/repeater_impl.cpp
	 repeater/repeater_reader.cpp)

set (test_files
	 ${main_files}
	 tests/test.cpp
	 tests/draft_journal_tests.cpp
	 tests/interval_type_tests.cpp
	 tests/ordinary_journal_tests.cpp
	 tests/phatbooks_tests_common.cpp
	 tests/repeater_tests.cpp)

add_executable (phatbooks main.cpp ${main_files})
add_executable (phatbooks_test ${test_files})

set (libraries
     boost_date_time
	 boost_filesystem
	 boost_locale-mt
	 boost_regex
	 boost_system
	 jewel 
	 unittest++
	 sqlite3
	 sqloxx)

target_link_libraries (phatbooks ${libraries})
target_link_libraries (phatbooks_test ${libraries})

if (MSYS)
	set (binary_installation_dir C:/MSYS/1.0/local/bin)
else ()
	set (binary_installation_dir bin)
endif ()

install(TARGETS phatbooks DESTINATION ${binary_installation_dir})
					


