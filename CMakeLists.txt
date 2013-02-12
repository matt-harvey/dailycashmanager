cmake_minimum_required (VERSION 2.6)

project (phatbooks_project)

set (immediate_directories
     .
	 account
	 application
	 commodity
	 date
	 entry
	 finformat
	 graphical_session
	 journal
	 output_aux
	 phatbooks_database_connection
	 phatbooks_persistent_object
	 phatbooks_text_session
	 repeater
	 session)

if (WIN32)
	include_directories (${immediate_directories}
	                     C:/Dev-Cpp
						 C:/Dev-Cpp/include/common
						 C:/include)
	link_directories(C:/Dev-Cpp/lib
	                 C:/Dev-Cpp/MinGW/lib
					 C:/lib)
	set (libraries
	     boost_date_time-mgw46-mt-1_53
		 boost_filesystem-mgw46-mt-1_53
		 boost_locale-mgw46-mt-1_53
		 boost_regex-mgw46-mt-1_53
		 boost_system-mgw46-mt-1_53
		 jewel
		 UnitTest++
		 sqloxx)
else ()
	include_directories(${immediate_directories})
	link_directories(/usr/local/lib)
	set (libraries
		 boost_date_time
		 boost_filesystem
		 boost_locale-mt
		 c
		 dl
		 boost_regex
		 boost_system
		 jewel 
		 m
		 pthread
		 UnitTest++
		 sqloxx)
endif ()


set (main_files
	 account/account.cpp
	 account/account_impl.cpp
	 account/account_reader.cpp
	 account/account_type.cpp
	 account/balance_cache.cpp
	 application/application.cpp
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
	 graphical_session/graphical_session.cpp
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
target_link_libraries (phatbooks ${libraries})

add_executable (phatbooks_test ${test_files})
target_link_libraries (phatbooks_test ${libraries})

if (WIN32)
	set (binary_installation_dir C:/bin)
else ()
	set (binary_installation_dir bin)
endif ()

install(TARGETS phatbooks DESTINATION ${binary_installation_dir})
					


